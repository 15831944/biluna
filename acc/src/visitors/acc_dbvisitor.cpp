/*****************************************************************
 * $Id: acc_dbvisitor.cpp 2166 2014-09-25 13:23:18Z rutger $
 * Created: Jan 16, 2012 12:53:43 PM - rutger
 *
 * Copyright (C) 2012 Red-Bag. All rights reserved.
 * This file is part of the Biluna ACC project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "acc_dbvisitor.h"

#include "acc_modelfactory.h"
#include "acc_objectfactory.h"
#include "acc_sqlcommonfunctions.h"
#include "rb_database.h"
#include "rb_objectatomic.h"
#include "rb_objectcontainer.h"
#include "rb_string.h"


/**
 * Constructor
 * @param device device to be written
 */
ACC_DbVisitor::ACC_DbVisitor() {
    mProjectId = "";
    mIsInitDone = false;
    mIsSysSettingExisting = false;
    mIsProjectSettingExisting = false;
    mMemoryResolveLevel = RB2::ResolveNone;
}

/**
 * Visit object and execute Create Read Update database action,
 * preparation for the deletion only because children need to be deleted first
 * @param obj object
 */
void ACC_DbVisitor::visitObject(RB_ObjectBase* obj) {
    mObject = obj;

    if (!mObject) {
        RB_DEBUG->error("ACC_DbVisitor::visitObject() object is NULL ERROR");
        return;
    }

    RB_String objName = obj->getName();

    switch (mAction) {
    case DbRead :
        // Acc_Project and project settings are normally
        if (objName == "ACC_Project" || ACC_OBJECTFACTORY->isProjectSetting(objName)) {
            dbRead();
        } else if (objName == "ACC_SysSettingList") {
            // only one placeholder object or row in database with parent 'none'
            // objectcontainer parent is set to NULL in order to read with container parent ID
            RB_ObjectBase* parent = obj->getParent();
            obj->setParent(NULL);
            obj->setValue("parent", "none");
            dbRead();
            obj->setParent(parent); // restore parent
        } else if (objName.endsWith("List", Qt::CaseInsensitive)
                   && (ACC_OBJECTFACTORY->isSystemSetting(objName))) {
            // all system settings have the parent 'syssetting'
            // objectcontainer parent is set to NULL in order to read with container parent ID
            RB_ObjectBase* parent = obj->getParent();

            if (parent && (objName != parent->getParent()->getName())) { // tree test
                obj->setParent(NULL);
                obj->setValue("parent", "syssetting");
            }

            dbRead();

            if (parent && (objName != parent->getParent()->getName())) {
                obj->setParent(parent); // restore parent
            }
        } else if (ACC_OBJECTFACTORY->isSystemSetting(objName)) {
            // not really necessary, because of RB_ObjectAtomic and read in above list
            dbRead();
        }
        break;
    case DbUpdate :
        // If XXX_Project, check if system settings exist and whether there is valid project ID
        if (objName.endsWith("_Project", Qt::CaseInsensitive)) {
            if (!isInitDone()) {
                init();
            }

            if (mProjectId.isEmpty()) {
                return; // database error
            }

            // NOTE: ACC_Project already selected, not to be inserted
            mObject->setId(mProjectId);
            return;
        }

        if (!isProjectSettingExisting()
                && ACC_OBJECTFACTORY->isProjectSetting(objName)
                && objName.endsWith("List")) {
            // currently only ACC_ChartMaster(List)
            dbUpdateList();
        } else if (isProjectSettingExisting()
                   && ACC_OBJECTFACTORY->isProjectSetting(objName)) {
            // skip branch, reset in leave object
            mMemoryResolveLevel = getResolveLevel();
            setResolveLevel(RB2::ResolveNone);
        } else if (objName == "ACC_SysSetting") {
            // to ensure parent is set to syssetting
            obj->setId("syssetting");
        } else if (!isSystemSettingExisting()
                   && ACC_OBJECTFACTORY->isSystemSetting(objName)
                   && objName.endsWith("List")) {
            dbUpdateList();
        } else if (isSystemSettingExisting()
                   && ACC_OBJECTFACTORY->isSystemSetting(objName)) {
            // skip branch
            mMemoryResolveLevel = getResolveLevel();
            setResolveLevel(RB2::ResolveNone);
        } else if (!ACC_OBJECTFACTORY->isProjectSetting(objName)
                   && !ACC_OBJECTFACTORY->isSystemSetting(objName)) {
            dbUpdateList();
        }

        break;
    case DbDelete :
        // dbBeforeDelete();
        break;
    case DbNone :
        RB_DEBUG->error("ACC_DbVisitor::visitObject() action not set ERROR");
        break;
    default :
        RB_DEBUG->error("ACC_DbVisitor::visitObject() action not set ERROR");
        break;
    }
}

void ACC_DbVisitor::leaveObject(RB_ObjectBase *obj) {
    if (mMemoryResolveLevel != RB2::ResolveNone) {
        setResolveLevel(mMemoryResolveLevel);
    } else {
        RB_DbVisitor::leaveObject(obj);
    }
}

/**
 * Update or insert objects from list into database
 * @param db database connection
 * @param level resolve level
 * @return true on success
 */
bool ACC_DbVisitor::dbUpdateList() {
    if (!mObject->isList()) {
        return dbUpdate();
    }

    mCalledFromList = true;

    bool isFirstUpdateObject = true;
    bool isFirstInsertObject = true;
    int memberCount = 0;
    bool success = true;

    QSqlDatabase db = RB_DATABASE->database(mDatabaseConnection);
    QSqlQuery qUpdate(db);
    QSqlQuery qInsert(db);
    RB_ObjectIterator* iter = mObject->createIterator();

    for (iter->first(); !iter->isDone() && success; iter->next()) {
        // This is a list container otherwise no objects
        RB_ObjectBase* obj = iter->currentObject();

        if (obj->getFlag(RB2::FlagFromDatabase)
                && obj->getFlag(RB2::FlagIsDeleted)) {
            RB_DbVisitor* vis = new RB_DbVisitor();
            vis->setDatabase(db);
            vis->setDbDelete();
            obj->acceptVisitor(*vis);
            delete vis;
            continue;
        }

        if(!obj->getFlag(RB2::FlagFromDatabase) && !obj->isList()) {
            // can be an object from outside
            if (obj->getValue("created").toString() == "0000-00-00T00:00:00") {
                obj->setValue("created", QDateTime::currentDateTime().toString(Qt::ISODate));
                obj->setValue("changed", QDateTime::currentDateTime().toString(Qt::ISODate));
            }

            if (isFirstInsertObject) {
                prepareInsertQuery(qInsert, obj);
                memberCount = obj->countMember();
                isFirstInsertObject = false;
            }

            for (int i = 0; i < memberCount; ++i) {
                RB_ObjectMember* mem = obj->getMember(i);
                if (mem->getName() == "id") {
                    if (!mIsUpdateNewId) {
                        qInsert.addBindValue(obj->getMember(i)->getValue(), QSql::In);
                    } else {
                        RB_String strId = RB_Uuid::createUuid().toString();

//                        if (!isSystemSettingExisting()
//                                && obj->getName() == "ACC_AccountGroup") {
//                            // This assumes that ACC_ChartMaster is always
//                            // before ACC_AccountGroup, find the new
//                            // ACC_ChartMaster ID corresponding to
//                            // the old ID from import XML file
//                            strId = findNewGroupId(mem->getValue().toString());
//                        }

                        // setValue() required for parent setting
                        obj->getMember(i)->setValue(strId);
                        qInsert.addBindValue(strId, QSql::In);
                    }
                } else if (mem->getName() == "parent") {
                    // make sure that children have correct parent ID
                    RB_ObjectBase* grParent = obj->getGrandParent();

                    if (grParent) {
                        qInsert.addBindValue(grParent->getId(), QSql::In);
                    }
                } else if (mem->getName().endsWith("_idx")) {
                    qInsert.addBindValue(obj->getMember(i)->getValue().toString()
                                         + obj->getMember(i)->getDisplayValue().toString(),
                                         QSql::In);
//                } else if (!isSystemSettingExisting()
//                           && obj->getName() == "ACC_ChartMaster"
//                           && mem->getName() == "accountgroup_id") {
//                    // Find (ACC_AccountGroup) ID already used in
//                    // previous accountgroup_id of ACC_ChartMaster
//                    RB_String strId = findNewGroupId(mem->getValue().toString());

//                    if (strId == "0") {
//                        // not yet used, not found
//                        strId = RB_Uuid::createUuid().toString();
//                        // Store old ID with new ACC_AccountGroup ID
//                        mGroupIdMap[mem->getValue().toString()] = strId;
//                    }

//                    qInsert.addBindValue(strId, QSql::In);
//                } else if (isSystemSettingExisting()
//                           && obj->getName() == "ACC_ChartMaster"
//                           && mem->getName() == "accountgroup_id") {
//                    qInsert.addBindValue("0", QSql::In);
                } else {
                    qInsert.addBindValue(mem->getValue(), QSql::In);
                }
            }

            if (qInsert.exec()) {
                obj->setFlag(RB2::FlagFromDatabase);
                obj->delFlag(RB2::FlagIsDirty);
            } else {
                RB_DEBUG->error("RB_DbVisitor::dbUpdate()1 ERROR");
                RB_DEBUG->print(qInsert.lastError().text());
                RB_DEBUG->print(qInsert.lastQuery());
                return false;
            }
        } else if (obj->getFlag(RB2::FlagFromDatabase) && !obj->isList()
                   && obj->getFlag(RB2::FlagIsDirty)) {
            // changed date and flag was already set

            if (isFirstUpdateObject) {
                prepareUpdateQuery(qUpdate, obj);
                memberCount = obj->countMember();
                isFirstUpdateObject = false;
            }

            for (int i = 1; i < memberCount; ++i) { // not ID
                RB_ObjectMember* mem = obj->getMember(i);
                if (mem->getName().endsWith("_idx")) {
                    qUpdate.addBindValue(obj->getMember(i)->getValue().toString()
                                         + obj->getMember(i)->getDisplayValue().toString(),
                                         QSql::In);
                } else {
                    qUpdate.addBindValue(obj->getMember(i)->getValue(), QSql::In);
                }

            }
            // ID is last in where statement
            qUpdate.addBindValue(obj->getMember(0)->getValue(), QSql::In);

            if (qUpdate.exec()) {
                obj->setFlag(RB2::FlagFromDatabase);
                obj->delFlag(RB2::FlagIsDirty);
            } else {
                RB_DEBUG->error("RB_DbVisitor::dbUpdate()1 ERROR");
                RB_DEBUG->print(qUpdate.lastError().text());
                RB_DEBUG->print(qUpdate.lastQuery());
                return false;
            }
        }
    }

    delete iter;
    return success;
}


/**
 * Initialize visitor: check system settings exist
 * and valid project ID available
 */
void ACC_DbVisitor::init() {
    mIsInitDone = true;

    if (!ACC_MODELFACTORY->getDatabase().isOpen()) {
        RB_DEBUG->error("ACC_DbVisitor::init() database not open ERROR");
        return;
    }

    mProjectId = ACC_MODELFACTORY->getRootId();
    // mObject->setId(mProjectId);

    ACC_SqlCommonFunctions f;
    QSqlQuery query(ACC_MODELFACTORY->getDatabase());
    f.selectAllFromWhere(query, "acc_accountgroup", "`parent`='syssetting'");

    if (query.first()) {
        mIsSysSettingExisting = true;
    }

    query.clear();
    f.selectAllFromWhere(query, "acc_chartmaster", "`parent`='" + mProjectId + "'");

    if (query.first()) {
        mIsProjectSettingExisting = true;
    }
}

/**
 * @returns true when initialization has been done: check system settings exist
 * and valid project ID available
 */
bool ACC_DbVisitor::isInitDone() {
    return mIsInitDone;
}

/**
 * @returns true if acc_syssetting already exists
 */
bool ACC_DbVisitor::isSystemSettingExisting() {
    return mIsSysSettingExisting;
}

/**
 * @returns true if acc_chartmaster already exists for this project
 */
bool ACC_DbVisitor::isProjectSettingExisting() {
    return mIsProjectSettingExisting;
}

/**
 * Find the account group ID created when inserting the ACC_ChartMaster records
 * @param groupId accountgroup_id from ACC_ChartMaster
 * @return ID (Uuid) for ACC_AccountGroup or '0' if not found
 */
//RB_String ACC_DbVisitor::findGroupId(const RB_String& groupId) {
//    std::map<RB_String, RB_String>::const_iterator iter;
//    iter = mGroupMap.find(groupId);

//    if (iter != mGroupMap.end()) {
//        return iter->second;
//    }

//    return "0";
//}
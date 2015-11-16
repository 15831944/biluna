/*****************************************************************
 * $Id: db_permissionhandler.h 2015-11-13 22:09:36Z rutger $
 * Created: Nov 13, 2015 22:09:36 PM - rutger
 *
 * Copyright (C) 2015 Red-Bag. All rights reserved.
 * This file is part of the Biluna DB project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "db_permissionhandler.h"

#include "db_objectfactory.h"
#include "db_sqlcommonfunctions.h"
#include "db_utilityfactory.h"

DB_PermissionHandler* DB_PermissionHandler::mActiveUtility = nullptr;


DB_PermissionHandler::DB_PermissionHandler() {
    RB_DEBUG->print("DB_PermissionHandler::DB_PermissionHandler()");
    mUserName = "";
    mPassword = "";
    mUserCount = 0;
    mPermissionList = nullptr;
    DB_UTILITYFACTORY->addUtility(this);
}

DB_PermissionHandler::~DB_PermissionHandler() {
    DB_UTILITYFACTORY->removeUtility(this);
    delete mPermissionList;
    mActiveUtility = nullptr;
    RB_DEBUG->print("DB_PermissionHandler::~DB_PermissionHandler() OK");
}

DB_PermissionHandler* DB_PermissionHandler::getInstance() {
    // SQLite SELECT datetime('now');
    // MySQL SELECT NOW(); or CURRDATE() CURTIME()
    if (!mActiveUtility) {
        mActiveUtility = new DB_PermissionHandler();
    }
    return mActiveUtility;
}

void DB_PermissionHandler::refresh() {
    setUserPermission();
}

void DB_PermissionHandler::setUserName(const QString& userName) {
    mUserName = userName;
}

void DB_PermissionHandler::setPassword(const QString& password) {
    mPassword = password;
}

void DB_PermissionHandler::setUserPermission() {
    setUserCount();

    // create per actual project with the user permission
    if (mPermissionList) {
        delete mPermissionList;
    }

    mPermissionList = new RB_ObjectContainer("", NULL,
                                             "DB_SystemUserPermissionList",
                                             DB_OBJECTFACTORY);
    // set user list
    DB_SqlCommonFunctions f;
    f.getPermissionList(mPermissionList, mUserName, mPassword);
}

bool DB_PermissionHandler::isValidDbUser() {
    if (mUserCount < 1) {
        // no system users in database
        return true;
    }

    if (mPermissionList->countObject() < 1) {
        // system users but no with username and password
        return false;
    }

    QDate today = QDate::currentDate();
    RB_ObjectIterator* iter = mPermissionList->createIterator();
    iter->first();
    RB_ObjectBase* user = iter->currentObject();
    QDate startDate = user->getValue("startdate").toDate();
    QDate endDate = user->getValue("enddate").toDate();

    if (startDate <= today && today >= endDate) {
        return true;
    }

    return false;
}

int DB_PermissionHandler::getPerspectivePermission(const RB_String& perspectiveCode) {
    // TODO


    RB2::PermissionType perm = RB2::PermissionNone;
    // at least an administrator and a user should be part of the permission list
    if (mUserCount < 2) {
        return perm;
    }

    RB_ObjectIterator* iter = mPermissionList->createIterator();

    for (iter->first(); !iter->isDone() && perm == RB2::PermissionNone;
         iter->next()) {
        RB_ObjectBase* obj = iter->currentObject();
        RB_String str = obj->getDValue("persproject_idx").toString();
        // PENG_Project - NEW001
        RB_StringList strList = str.split("_");
        RB_String perspCode = strList.at(0);

        if (perspCode.compare(perspectiveCode, Qt::CaseInsensitive) == 0) {
            perm = RB2::PermissionDefault;
        }
    }

    delete iter;
    return perm;
}

int DB_PermissionHandler::getCrudxPermission(const RB_String& modelName,
                                        const RB_String& projectId) {
    RB_StringList modelList = modelName.split("_", QString::SkipEmptyParts);

    if (modelList.size() < 2) {
        RB_DEBUG->error("DB_PermissionHandler::getCrudxPermission() method name ERROR");
        return (int)RB2::PermissionNone;
    }

    // Check perspective
    int permissionGranted = 0; // userHasPerspectivePermission(modelList.at(0));

    // TODO: class level, method level
    // ...

    return permissionGranted;
}

void DB_PermissionHandler::setUserCount() {
    DB_SqlCommonFunctions f;
    mUserCount = f.getUserCount();
}

void DB_PermissionHandler::handlePermissionSetting(RB_ObjectBase* projectObj,
                                                   RB_ObjectBase* permObj) {
    // update

    //    userstart
    //    userend
    //    groupstatus_id live locked hidden
    //    grouppermission_id none r ru crud crudx
    //    groupstart
    //    groupend
    //    persproject_idx {Uuid}tablename - project number
    //    persprojectstatus_id live locked hidden test
    //    persprojectstart
    //    persprojectend

        // continue here ...
    // if project userstart < permission userstart : permission userstart
    if (projectObj->getValue("userstart").toString()
            < permObj->getValue("userstart").toString()) {
        projectObj->setValue("userstart", permObj->getValue("userstart"));
    }

    // if project userend > permission userend : permission userend
    if (projectObj->getValue("userend").toString()
            > permObj->getValue("userend").toString()) {
        projectObj->setValue("userend", permObj->getValue("userend"));
    }

    // TODO: Remove? Makes the permission flexible but also complicated?
    // largest of groupstatus_id live=open locked=r only hidden=none
    if (projectObj->getValue("groupstatus_id").toInt()
            < permObj->getValue("groupstatus_id").toInt()) {
        projectObj->setValue("groupstatus_id", permObj->getValue("groupstatus_id"));
    }

    // largest of grouppermission_id r ru crud crudx
    if (projectObj->getValue("grouppermission_id").toInt()
            < permObj->getValue("grouppermission_id").toInt()) {
        projectObj->setValue("grouppermission_id", permObj->getValue("grouppermission_id"));
    }

    // if project groupstart < permission groupstart : project groupstart
    if (projectObj->getValue("groupstart").toString()
            < permObj->getValue("groupstart").toString()) {
        projectObj->setValue("groupstart", permObj->getValue("groupstart"));
    }

    // if project groupend > permission groupend : project groupend
    if (projectObj->getValue("groupend").toString()
            > permObj->getValue("groupend").toString()) {
        projectObj->setValue("groupend", permObj->getValue("groupend"));
    }

    // persproject_idx do nothing

    // smallest of persprojectstatus_id (should all be the same)
    if (projectObj->getValue("groupstatus_id").toInt()
            > permObj->getValue("groupstatus_id").toInt()) {
        projectObj->setValue("groupstatus_id", permObj->getValue("groupstatus_id"));
    }

    // if project persprojectstart < permission persprojectstart : permission persprojectstart (should all be the same)
    if (projectObj->getValue("persprojectstart").toString()
            < permObj->getValue("persprojectstart").toString()) {
        projectObj->setValue("persprojectstart", permObj->getValue("persprojectstart"));
    }

    // if project persprojectend > permission persprojectend : permission persprojectend (should all be the same)
    if (projectObj->getValue("persprojectend").toString()
            > permObj->getValue("persprojectend").toString()) {
        projectObj->setValue("persprojectend", permObj->getValue("persprojectend"));
    }

    RB_DEBUG->print(projectObj->toString());
}




﻿#include "rb_report.h"
#include "rb_objectatomic.h"
NAMESPACE_REDBAG_CALC

RB_Report::RB_Report(RB_ObjectContainer* inputOutput) {
    if (!inputOutput) {
        RB_DEBUG->warning("RB_Report::RB_Report() inputOutput WARNING");
        mOutContainer = NULL;
        return;
    }

    mInContainer = inputOutput->getContainer("PCALC_InputList");
    mOutContainer = inputOutput->getContainer("PCALC_OutputList");
    mLastOutput = NULL;
}

RB_Report::~RB_Report() {
    // nothing yet
}

void RB_Report::addDetail(const QString& formulaNumber,
                       const QString& variableName,
                       const QString& formula,
                       double result,
                       const QString& unit,
                       const QString& formulaValues,
                       int loadCaseNo,
                       const QString& note) {

    if (!mInContainer || !mOutContainer) {
        RB_DEBUG->error("RB_Report::addDetail() input or output container ERROR");
        return;
    }

    RB_ObjectBase* settingObj = mInContainer->getObject("name", "PCALC_Setting");

    if (!settingObj) {
        RB_DEBUG->error("RB_Report::addDetail() setting object ERROR");
        return;
    }

    // Report filter settings
    if (settingObj->getValue("lastvalue").toInt() == 1) {
        RB_ObjectBase* existObj = getObject(mOutContainer, variableName,
                                            loadCaseNo);
        if (existObj) {
            existObj->setValue("formulanumber", formulaNumber);
            existObj->setValue("variablename", variableName);
            existObj->setValue("formula", formula);
            existObj->setValue("result", result);
            existObj->setValue("unit", unit);
            existObj->setValue("formulavalues", formulaValues);
            existObj->setValue("loadcaseno", loadCaseNo);
            existObj->setValue("note", note);
            return;
        }
    }

    int from = settingObj->getValue("formulafrom").toInt();
    int to = settingObj->getValue("formulato").toInt();
    RB_String formulaStr = "Formula ";

    if ((formulaNumber < formulaStr + qn(from) && from > 0)
            || (formulaStr + qn(to) < formulaNumber && to < 999)) {
        return;
    }

    // Add result to output object and container
    bool withoutMembers = true;
    RB_ObjectAtomic* obj = new RB_ObjectAtomic("", mOutContainer,
                                               "PCALC_Output", NULL,
                                               withoutMembers);
    obj->addMember("formulanumber", "-", formulaNumber,
                   RB2::MemberChar125);
    obj->addMember("variablename", "-", variableName,
                   RB2::MemberChar125);
    obj->addMember("formula", "-", formula,
                   RB2::MemberChar2500);
    obj->addMember("result", unit, result,
                   RB2::MemberDouble);
    obj->addMember("unit", "-", unit,
                   RB2::MemberChar125);
    obj->addMember("formulavalues", "-", formulaValues,
                   RB2::MemberChar2500);
    obj->addMember("loadcaseno", "-", loadCaseNo,
                   RB2::MemberInteger);
    obj->addMember("note", "-", note,
                   RB2::MemberChar165);
    mOutContainer->addObject(obj);
    mLastOutput = obj;
}

RB_ObjectBase *RB_Report::getLastOutput() {
    return mLastOutput;
}


RB_String RB_Report::qn(double val) {
    return RB_String::number(val);
}

RB_String RB_Report::fv(const char* format ...) {
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsprintf_s (buffer,format, args);
    RB_String strBuffer(buffer);
    va_end (args);

    return strBuffer;
}

RB_ObjectBase* RB_Report::getObject(RB_ObjectContainer* outContainer,
                                    const QString& variableName,
                                    int loadCaseNo) {
    RB_ObjectIterator* iter = outContainer->createIterator();

    for (iter->first(); !iter->isDone(); iter->next()) {
        RB_ObjectBase* obj = iter->currentObject();

        if (obj->getValue("variablename").toString() == variableName
                && obj->getValue("loadcaseno").toInt() == loadCaseNo) {
            return obj;
        }
    }

    return NULL;
}

END_NAMESPACE_REDBAG_CALC

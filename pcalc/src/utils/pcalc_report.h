﻿#ifndef RB_REPORT_H
#define RB_REPORT_H

#include <cmath>
#include "rb_namespace.h"
#include "rb_objectcontainer.h"
#include "rb_utility.h"

#define PR Biluna::Calc::PCALC_Report::getInstance()
#define QN(value) RB_String::number(value)

NAMESPACE_BILUNA_CALC

class PCALC_Report : public RB_Utility {

public:
    virtual ~PCALC_Report();
    static PCALC_Report* getInstance();

    void refresh() {}

    virtual void setReportSettings(int reportType,
                                   int formulaFrom,
                                   int formulaTo);
    virtual void addDetail(const QString& formulaNumber,
                           const QString& variableName,
                           const QString& formula,
                           double result,
                           const QString& unit,
                           const QString& formulaValues = "",
                           int loadCaseNo = -1,
                           const QString& note = "");
    RB_ObjectContainer* getInOutContainer();

    /**
     * Used for unittest when calculation creates output object, can
     * be used only once. The next time a new output object is created.
     * This new object is required for functions that do not create a
     * calculation output object
     * @brief Used for unittest
     * @return last calculation output object
     */
    RB_ObjectBase* getLastOutput();
    virtual void clear();

private:
    PCALC_Report();
    void createInputOutputObject();
    RB_ObjectBase* getObject(RB_ObjectContainer* outContainer,
                             const QString& formulaNumber,
                             const QString& variableName,
                             int loadCaseNo);

    static PCALC_Report* mActiveUtility;
    RB_ObjectContainer* mInOutContainer;
    RB_ObjectContainer* mInContainer;
    RB_ObjectContainer* mOutContainer;
    RB_ObjectBase* mLastOutput;

    int mReportType;
    int mFormulaFrom;
    int mFormulaTo;
};

END_NAMESPACE_BILUNA_CALC

#endif //RB_REPORT_H



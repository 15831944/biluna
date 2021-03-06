/*****************************************************************
 * $Id: peng_lineprocess.cpp 2155 2014-08-04 16:50:28Z rutger $
 * Created: 2004 - rutger
 *
 * Copyright (C) 2004-2005 Red-Bag. All rights reserved.
 * This file is part of the Biluna PENG project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "peng_lineprocess.h"


/**
 * Constructor
 * @param id Uuid universal unique identification
 * @param p parent object container
 * @param n name of this object, usually PENG_LineProcess
 * @param f object factory interface, used for cloning and copying of object
 */
PENG_LineProcess::PENG_LineProcess (const RB_String& id, RB_ObjectBase* p,
                    const RB_String& n, RB_ObjectFactory* f)
                    : RB_ObjectContainer (id, p, n, f) {
	
    createMembers();
}


PENG_LineProcess::PENG_LineProcess(PENG_LineProcess* line) : RB_ObjectContainer(line) {
    createMembers();
    *this = *line;
}


PENG_LineProcess::~PENG_LineProcess() {
    // clean up children done in RB_ObjectBase and RB_ObjectContainer
}

/**
 * Create members:
 * - processfluiddescription
 * - processoperatingpress
 * - processoperatingtempstd
 * - processoperatingtemphigh
 * - processoperatingtemplow
 * - processtestmedium
 * - processtestpress
 * - processpressdrop
 * - processmassflowratestd
 * - processmassflowratemax
 * - processvelocity
 * - processphase
 * - processboilingpoint
 * - processvapourpress
 * - processmolweight
 * - processdensityvapour
 * - processdensityliquid
 * - processviscosityvapour
 * - processviscosityliquid
 * - processstandardvolumeflowstd
 * - processstandardvolumeflowmax
 * - processactualflowstd
 * - processactualflowmax
 * - processsimulation
 */
void PENG_LineProcess::createMembers() {
    addMember("processfluiddescription", "-", "", RB2::MemberChar125);
    addMember("processoperatingpress", "MPa", 0.0, RB2::MemberDouble);
    addMember("processoperatingtempstd", "DegC", 0.0, RB2::MemberDouble);
    addMember("processoperatingtemphigh", "DegC", 0.0, RB2::MemberDouble);
    addMember("processoperatingtemplow", "DegC", 0.0, RB2::MemberDouble);
    addMember("processtestmedium", "-", "", RB2::MemberChar125);
    addMember("processtestpress", "MPa", 0.0, RB2::MemberDouble);
    addMember("processpressdrop", "MPa", 0.0, RB2::MemberDouble);
    addMember("processmassflowratestd", "kg/s", 0.0, RB2::MemberDouble);
    addMember("processmassflowratemax", "kg/s", 0.0, RB2::MemberDouble);
    addMember("processvelocity", "m/s", 0.0, RB2::MemberDouble);
    addMember("processphase_id", "-", 0, RB2::MemberInteger);
    addMember("processboilingpoint", "DegC", 0.0, RB2::MemberDouble);
    addMember("processvapourpress", "MPa", 0.0, RB2::MemberDouble);
    addMember("processmolweight", "-", 0.0, RB2::MemberDouble);
    addMember("processdensityvapour", "kg/m3", 0.0, RB2::MemberDouble);
    addMember("processdensityliquid", "kg/m3", 0.0, RB2::MemberDouble);
    addMember("processviscosityvapour", "cP", 0.0, RB2::MemberDouble);
    addMember("processviscosityliquid", "cP", 0.0, RB2::MemberDouble);
    addMember("processstandardvolumeflowstd", "m3/h", 0.0, RB2::MemberDouble);
    addMember("processstandardvolumeflowmax", "m3/h", 0.0, RB2::MemberDouble);
    addMember("processactualflowstd", "m3/h", 0.0, RB2::MemberDouble);
    addMember("processactualflowmax", "m3/h", 0.0, RB2::MemberDouble);
    addMember("processsimulation", "-", "", RB2::MemberChar125);
}

/**
 * interception of RB_ObjectBase::setValue() to set the previousValue 
 * if previousValue was not set before with original value
 */
void PENG_LineProcess::setValue(int number, const RB_Variant& var) {
    if (!RB_ObjectBase::getPValue(number).isValid()) {
        RB_Variant mVar = RB_ObjectBase::getValue(number);

        if (mVar.isValid()) {
            RB_ObjectBase::setPValue(number, mVar);
        } else {
            RB_DEBUG->print("PENG_LineProcess::setValue(number, var) number: "
                            + QString::number(number) + " does not exist ERROR");
            return;
        }
    }

    RB_ObjectBase::setValue(number, var);
}

/**
 * interception of RB_ObjectBase::setValue() to set the previousValue 
 * if previousValue was not set before with original value
 */
void PENG_LineProcess::setValue(const RB_String& name, const RB_Variant& var) {
    if (!RB_ObjectBase::getPValue(name).isValid()) {
        RB_Variant mVar = RB_ObjectBase::getValue(name);

        if (mVar.isValid()) {
            RB_ObjectBase::setPValue(name, mVar);
        } else {
            // error
            RB_DEBUG->print("PENG_LineProcess::setValue(name, var) name: "
                            + name + " does not exist ERROR");
            return;
        }
    }

    RB_ObjectBase::setValue(name, var);
}


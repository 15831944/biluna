/*****************************************************************
 * $Id: pcalc_component.cpp 1826 2012-12-08 00:31:33Z rutger $
 * Created: Apr 7, 2015 - rutger
 *
 * Copyright (C) 2015 Biluna. All rights reserved.
 * This file is part of the Biluna PENG project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "pcalc_en1591_shell.h"


PCALC_EN1591_Shell::PCALC_EN1591_Shell(const RB_String& id, RB_ObjectBase* p,
                                     const RB_String& n, RB_ObjectFactory* f)
                : RB_ObjectAtomic(id, p, n, f) {
	 createMembers();
}

PCALC_EN1591_Shell::PCALC_EN1591_Shell(PCALC_EN1591_Shell* component)
                : RB_ObjectAtomic(component) {
	createMembers();
	*this = *component;
}

PCALC_EN1591_Shell::~PCALC_EN1591_Shell() {
    // nothing
}

/**
 * Create members:
 * \li -
 */
void PCALC_EN1591_Shell::createMembers() {
    // flange/shell 1
    addMember(this, "typeshell1_id", "-", 0, RB2::MemberInteger);
    addMember(this, "ds1", "mm", 0.0, RB2::MemberDouble);
    addMember(this, "es1", "mm", 0.0, RB2::MemberDouble);
    addMember(this, "phis1", "degree", 0.0, RB2::MemberDouble);
    addMember(this, "materialshell1_idx", "-", "0", RB2::MemberChar165);
    // flange/shell 2
    addMember(this, "shell2equal", "-", 0, RB2::MemberInteger);
    addMember(this, "typeshell2_id", "-", 0, RB2::MemberInteger);
    addMember(this, "ds2", "mm", 0.0, RB2::MemberDouble);
    addMember(this, "es2", "mm", 0.0, RB2::MemberDouble);
    addMember(this, "phis2", "degree", 0.0, RB2::MemberDouble);
    addMember(this, "materialshell2_idx", "-", "0", RB2::MemberChar165);
}

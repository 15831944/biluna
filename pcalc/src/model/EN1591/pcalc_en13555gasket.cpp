/*****************************************************************
 * $Id: pcalc_en13555gasket.cpp 0001 2016-01-25T19:20:30 rutger $
 *
 * Copyright (C) 2016 Red-Bag. All rights reserved.
 * This file is part of the Biluna PCALC project.
 *
 * See http://www.biluna.com for further details.
 *****************************************************************/

#include "pcalc_en13555gasket.h"

PCALC_EN13555Gasket::PCALC_EN13555Gasket(const QString& id, RB_ObjectBase* p,
                    const QString& n, RB_ObjectFactory* f)
					: RB_ObjectContainer (id, p, n, f) {
	createMembers();
}

PCALC_EN13555Gasket::PCALC_EN13555Gasket(PCALC_EN13555Gasket* obj)
                    : RB_ObjectContainer(obj) {
	createMembers();
    *this = *obj;
}

PCALC_EN13555Gasket::~PCALC_EN13555Gasket() {
	// clean up children done in RB_ObjectBase and RB_ObjectContainer
}

void PCALC_EN13555Gasket::createMembers() {
    addMember("type", "-", "", RB2::MemberChar125);
    addMember("testoutdiam", "mm", 0.0, RB2::MemberDouble);
    addMember("testindiam", "mm", 0.0, RB2::MemberDouble);
    addMember("issue", "-", "1970-01-01", RB2::MemberChar10);
}
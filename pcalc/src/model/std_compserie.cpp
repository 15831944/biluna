/*****************************************************************
 * $Id: std_compserie.cpp 0001 2016-07-13T15:43:00 rutger $
 *
 * Copyright (C) 2016 Red-Bag. All rights reserved.
 * This file is part of the Biluna STD project.
 *
 * See http://www.biluna.com for further details.
 *****************************************************************/

#include "std_compserie.h"

STD_CompSerie::STD_CompSerie(
                   const QString& id, RB_ObjectBase* p,
                   const QString& n, RB_ObjectFactory* f)
					: RB_ObjectAtomic (id, p, n, f) {
	createMembers();
}

STD_CompSerie::STD_CompSerie(STD_CompSerie* obj) :
                   RB_ObjectAtomic(obj) {
	createMembers();
	*this = *obj;
}

STD_CompSerie::~STD_CompSerie() {
	// clean up children done in RB_ObjectBase and RB_ObjectContainer
}

/**
 * Create members:
 * \li type facing type number or name
 * \li description description of the facing type
 */
void STD_CompSerie::createMembers() {
    addMember("type", "-", "<NEW>", RB2::MemberChar125);
    addMember("description", "-", "", RB2::MemberChar125);
}

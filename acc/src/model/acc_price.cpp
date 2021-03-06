/*****************************************************************
 * $Id: acc_price.cpp 1983 2013-09-02 18:10:02Z rutger $
 * Created: Jan 20, 2010 10:04:25 AM - rutger
 *
 * Copyright (C) 2010 Red-Bag. All rights reserved.
 * This file is part of the Biluna ACC project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "acc_price.h"

ACC_Price::ACC_Price (const RB_String& id, RB_ObjectBase* p,
                    const RB_String& n, RB_ObjectFactory* f)
                    : RB_ObjectContainer (id, p, n, f) {
    createMembers();
}


ACC_Price::ACC_Price(ACC_Price* obj) : RB_ObjectContainer(obj) {
    createMembers();
    *this = *obj;
}


ACC_Price::~ACC_Price() {
    // clean up children done in RB_ObjectBase and RB_ObjectContainer
}

/**
 * Create members:
 * - salestype_id sales type or price list ID
 * - currency_id currency ID
 * - price price value
 */
void ACC_Price::createMembers() {
    addMember("salestype_id", "-", "0", RB2::MemberChar40);
    addMember("currency_id", "-", "0", RB2::MemberChar40);
    addMember("price", "-", 0.0, RB2::MemberDouble);
}

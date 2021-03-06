/*****************************************************************
 * $Id: acc_asset.cpp 1983 2013-09-02 18:10:02Z rutger $
 * Created: Oct 24, 2011 1:24:25 PM - rutger
 *
 * Copyright (C) 2011 Red-Bag. All rights reserved.
 * This file is part of the Biluna ACC project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "acc_asset.h"


ACC_Asset::ACC_Asset (const RB_String& id, RB_ObjectBase* p,
                    const RB_String& n, RB_ObjectFactory* f)
                    : RB_ObjectContainer (id, p, n, f) {
    createMembers();
}


ACC_Asset::ACC_Asset(ACC_Asset* obj) : RB_ObjectContainer(obj) {
    createMembers();
    *this = *obj;
}


ACC_Asset::~ACC_Asset() {
    // nothing
}

/**
 * Create members:
 * - assetcode code for this asset
 * - mname name
 * - type_id type ID of this asset
 * - data1 data line such as an building address of license plate of a car
 * - data2 data line
 * - data3 data line
 * - data4 data line
 * - data5 data line
 * - data6 data line
 * - lat latitude for building
 * - lng longitude for building
 * - salesman_id ID of salesman
 * - assetsince date when this asset was entered in the accounting
 * - contactname name of contact
 * - email email address of contact
 * - phoneno phone number
 * - faxno fax number
 * - note text
 */
void ACC_Asset::createMembers() {
    addMember("assetcode", "-", "<NEW>", RB2::MemberChar40);
    addMember("mname", "-", "", RB2::MemberChar125);
    addMember("type_id", "-", "0", RB2::MemberChar40);
    addMember("data1", "-", "", RB2::MemberChar125); // for example street address
    addMember("data2", "-", "", RB2::MemberChar125);
    addMember("data3", "-", "", RB2::MemberChar125);
    addMember("data4", "-", "", RB2::MemberChar125);
    addMember("data5", "-", "", RB2::MemberChar125);
    addMember("data6", "-", "", RB2::MemberChar40);

    addMember("lat", "-", 0.0, RB2::MemberDouble); // latitude (GeoCoordinate)
    addMember("lng", "-", 0.0, RB2::MemberDouble); // longitude (GeoCoordinate)

    addMember("salesman_id", "-", "", RB2::MemberChar40);
    addMember("assetsince", "-", "", RB2::MemberDateTime);

    addMember("contactname", "-", "", RB2::MemberChar125);
    addMember("email", "-", "", RB2::MemberChar125);
    addMember("phoneno", "-", "", RB2::MemberChar20);
    addMember("faxno", "-", "", RB2::MemberChar20);

    addMember("note", "-", "", RB2::MemberChar2500);
}

/*****************************************************************
 * $Id: srm_template.cpp 2071 2014-01-22 21:56:32Z rutger $
 * Created: Mar 06, 2012 14:24:25 AM - rutger
 *
 * Copyright (C) 2012 Red-Bag. All rights reserved.
 * This file is part of the Biluna CRM project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "srm_template.h"

SRM_Template::SRM_Template (const RB_String& id, RB_ObjectBase* p,
                    const RB_String& n, RB_ObjectFactory* f)
                    : RB_ObjectAtomic (id, p, n, f) {
    createMembers();
}


SRM_Template::SRM_Template(SRM_Template* obj) : RB_ObjectAtomic(obj) {
    createMembers();
    *this = *obj;
}


SRM_Template::~SRM_Template() {
    // clean up children done in RB_ObjectBase and RB_ObjectContainer
}

/*!
    Create members. The members are (name, unit, default value, type):
    \li type_id, -, 0, RB2::MemberInteger
    \li reference, -, '', RB2::MemberChar40
    \li standard, -, '', RB2::MemberInteger
    \li description, -, <NEW>, RB2::MemberChar125
    \li lastused, -, 1900-01-01, RB2::MemberChar20
    \li standard, -. 0, RB2::MemberInteger
    \li pageheader, -, '', RB2::MemberString
    \li doccontent, -, '', RB2::MemberString
    \li pagefooter, -, '', RB2::MemberString
    \li marginleft, mm, 15.0, RB2::MemberDouble
    \li marginright, mm, 10.0, RB2::MemberDouble
    \li margintop, mm, 10.0, RB2::MemberDouble
    \li marginbottom, mm, 10.0, RB2::MemberDouble
    \li pageheaderheight, mm, 15.0, RB2::MemberString
    \li pagefooterheight, mm, 10, RB2::MemberString

 */
void SRM_Template::createMembers() {
    addMember("type_id", "-", 0, RB2::MemberInteger);
    addMember("reference", "-", "", RB2::MemberChar40);
    addMember("standard", "-", 0, RB2::MemberInteger);
    addMember("description", "-", "<NEW>", RB2::MemberChar125);
    addMember("lastused", "-", "1900-01-01", RB2::MemberChar20);

    addMember("pageheader", "-", "", RB2::MemberString);
    addMember("doccontent", "-", "", RB2::MemberString);
    addMember("pagefooter", "-", "", RB2::MemberString);

    addMember("papersize", "-", 0, RB2::MemberInteger);

    addMember("marginleft", "mm", 15.0, RB2::MemberDouble);
    addMember("marginright", "mm", 10.0, RB2::MemberDouble);
    addMember("margintop", "mm", 10.0, RB2::MemberDouble);
    addMember("marginbottom", "mm", 10.0, RB2::MemberDouble);

    addMember("pageheaderheight", "mm", 20.0, RB2::MemberDouble);
    addMember("pagefooterheight", "mm", 20.0, RB2::MemberDouble);
}

/*****************************************************************
 * $Id: srm_campaign.cpp 2100 2014-02-18 19:55:20Z rutger $
 * Created: Feb 12, 2012 14:24:25 AM - rutger
 *
 * Copyright (C) 2012 Red-Bag. All rights reserved.
 * This file is part of the Biluna CRM project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "srm_campaign.h"

SRM_Campaign::SRM_Campaign (const RB_String& id, RB_ObjectBase* p,
                    const RB_String& n, RB_ObjectFactory* f)
                    : RB_ObjectContainer (id, p, n, f) {
    createMembers();
}


SRM_Campaign::SRM_Campaign(SRM_Campaign* obj) : RB_ObjectContainer(obj) {
    createMembers();
    *this = *obj;
}


SRM_Campaign::~SRM_Campaign() {
    // clean up children done in RB_ObjectBase and RB_ObjectContainer
}

/**
 * Create members:
 * - campaigntype_id campaign type ID such as letter or email
 * - campaingcode code by user for easy reference of campaign, usually capitals
 * - subject subject of campaing
 * - mstart start date/time of campaign
 * - mend end date/time of campaign
 * - currency_id currency ID
 * - budget money budget for this campaign
 * - expectedrevenue expected revenue of this campaing
 * - campaingstatus_id status such as ongoing and completed
 * - objective description of the goal
 * - description description of the campaign
 * - document_idx document ID and description for mailing campaign
 */
void SRM_Campaign::createMembers() {
    addMember("campaigntype_id", "-", 0, RB2::MemberInteger);
    addMember("campaigncode", "-", "<NEW>", RB2::MemberChar20);
    addMember("subject", "-", "", RB2::MemberChar125);
    addMember("mstart", "-", "", RB2::MemberDateTime);
    addMember("mend", "-", "", RB2::MemberDateTime);
    addMember("currency_id", "-", "0", RB2::MemberChar40);
    addMember("budget", "-", 0.0, RB2::MemberDouble);
    addMember("expectedrevenue", "-", 0.0, RB2::MemberDouble);
    addMember("campaignstatus_id", "-", 0, RB2::MemberInteger);
    addMember("objective", "-", "", RB2::MemberChar255);
    addMember("description", "-", "", RB2::MemberChar2500);
    addMember("document_idx", "-", "0", RB2::MemberChar125);
}

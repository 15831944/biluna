/*****************************************************************
 * $Id: crm_actionactivity.cpp 2156 2014-08-13 10:20:33Z rutger $
 * Created: Feb 12, 2012 14:24:25 AM - rutger
 *
 * Copyright (C) 2012 Red-Bag. All rights reserved.
 * This file is part of the Biluna CRM project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "crm_actionactivity.h"

#include "crm_dialogfactory.h"
#include "crm_modelfactory.h"
#include "rb_mdiwindow.h"


CRM_ActionActivity::CRM_ActionActivity()
        : RB_Action() {
    // nothing
}


RB_GuiAction* CRM_ActionActivity::createGuiAction() {
    static RB_GuiAction* ga = NULL;

    if (!ga) {
        ga = new RB_GuiAction(CRM_ActionActivity::getName(),
                              QIcon(":/images/icons/page_white_gear_crm.png"),
                              tr("&Activity"));
        ga->setStatusTip(tr("Activity"));
        ga->setFactory(CRM_ActionActivity::factory);
    }
    return ga;
}

RB_Action* CRM_ActionActivity::factory() {
    RB_Action* a = new CRM_ActionActivity();
    // no graphicsView with eventhandler which deletes the action
    a->trigger();
    delete a;
    a = NULL;
    return a;
}

/**
 * Trigger this action, which is done after all data and objects are set
 */
void CRM_ActionActivity::trigger() {
    // Check required settings
    if (CRM_MODELFACTORY->getRootId() == ""
            || !CRM_MODELFACTORY->getDatabase().isOpen()) {
        CRM_DIALOGFACTORY->requestWarningDialog(tr("No project selected.\n"
                                                   "Connect first to database\n"
                                                   "and then select project."));
        return;
    }

    // Create widget
    RB_MdiWindow* mdiWin = CRM_DIALOGFACTORY->getMdiWindow(
            CRM_DialogFactory::WidgetActivity);

    mdiWin->show();
    mdiWin->raise();
}

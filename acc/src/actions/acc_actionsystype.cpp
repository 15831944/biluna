/*****************************************************************
 * $Id: acc_actionsystype.cpp 2210 2015-01-22 14:59:25Z rutger $
 * Created: Feb 8, 2012 8:24:42 PM - rutger
 *
 * Copyright (C) 2012 Red-Bag. All rights reserved.
 * This file is part of the Biluna ACC project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "acc_actionsystype.h"

#include "acc_dialogfactory.h"
#include "acc_modelfactory.h"
#include "rb_mdiwindow.h"


ACC_ActionSysType::ACC_ActionSysType()
        : RB_Action() {
    // nothing
}


RB_GuiAction* ACC_ActionSysType::createGuiAction() {
    static RB_GuiAction* ga = NULL;

    if (!ga) {
        ga = new RB_GuiAction(ACC_ActionSysType::getName(),
                              //QIcon(":/images/icons/star.png"),
                              tr("S&ystem Sequence Numbers"));
        ga->setStatusTip(tr("System sequence number maintenance"));
        ga->setFactory(ACC_ActionSysType::factory);
    }
    return ga;
}

RB_Action* ACC_ActionSysType::factory() {
    RB_Action* a = new ACC_ActionSysType();
    // no graphicsView with eventhandler which deletes the action
    a->trigger();
    delete a;
    a = NULL;
    return a;
}

/**
 * Trigger this action, which is done after all data and objects are set
 */
void ACC_ActionSysType::trigger() {
    // Check required settings
    if (ACC_MODELFACTORY->getRootId() == ""
            || !ACC_MODELFACTORY->getDatabase().isOpen()) {
        ACC_DIALOGFACTORY->requestWarningDialog(tr("No project selected.\n"
                                                   "Connect first to database\n"
                                                   "and then select project."));
        return;
    }

    // Create dialog
    RB_Dialog* dlg = ACC_DIALOGFACTORY->getDialog(ACC_DialogFactory::DialogSysType);
    dlg->exec();
    dlg->deleteLater();
}

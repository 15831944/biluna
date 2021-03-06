/*****************************************************************
 * $Id: db_actionsystemversion.cpp 2210 2015-01-22 14:59:25Z rutger $
 * Created: Sep 2, 2013 1:57:42 AM - rutger
 *
 * Copyright (C) 2013 Red-Bag. All rights reserved.
 * This file is part of the Biluna DB project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "db_actionsystemversion.h"

#include "db_dialogfactory.h"
#include "db_modelfactory.h"
#include "rb_database.h"
#include "rb_dialogwindow.h"
#include "rb_mainwindow.h"


DB_ActionSystemVersion::DB_ActionSystemVersion()
        : RB_Action() {
    // nothing
}


RB_GuiAction* DB_ActionSystemVersion::createGuiAction() {
    static RB_GuiAction* ga = NULL;

    if (!ga) {
        ga = new RB_GuiAction(DB_ActionSystemVersion::getName(),
                                  "dialog",
                                  tr("&View database version"));
        ga->setFactory(DB_ActionSystemVersion::factory);
    }
    return ga;
}

RB_Action* DB_ActionSystemVersion::factory() {
    RB_Action* a = new DB_ActionSystemVersion();
    // no graphicsView with eventhandler which deletes the action
    a->trigger();
    delete a;
    a = NULL;
    return a;
}

/**
 * Trigger this action, which is done after all data and objects are set
 */
void DB_ActionSystemVersion::trigger() {
    // Check required settings
    if (DB_MODELFACTORY->getRootId() == ""
            || !DB_MODELFACTORY->getDatabase().isOpen()) {
        DB_DIALOGFACTORY->requestWarningDialog(tr("No project selected.\n"
                                                   "Connect first to database\n"
                                                   "and select project."));
        return;
    }

    // Create dialog
    RB_DialogWindow* dlg = DB_DIALOGFACTORY->getDialogWindow(
                DB_DialogFactory::WidgetVersion);
    dlg->exec();
    dlg->deleteLater();
}

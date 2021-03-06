/****************************************************************************
** $Id: rs_actionselectinvert.cpp 8821 2008-02-02 17:31:21Z andrew $
**
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
** This file is part of the qcadlib Library project.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid qcadlib Professional Edition licenses may use 
** this file in accordance with the qcadlib Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.ribbonsoft.com for further details.
**
** Contact info@ribbonsoft.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "rs_actionselectinvert.h"
#include "rs_selection.h"

RS_ActionSelectInvert::RS_ActionSelectInvert(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_ActionSelectBase(container, graphicView) {
}


RS_GuiAction* RS_ActionSelectInvert::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionSelectInvert::getName(), 
            "selectinvert",
            tr("&Invert Selection"));
        action->setStatusTip(tr("Inverts the current selection"));
        action->setCommand("selectinvert,ti");
        action->setKeycode("ti");
        action->setFactory(RS_ActionSelectInvert::factory);
    }                                   
    return action;
}


void RS_ActionSelectInvert::init(int status) {
    RS_ActionSelectBase::init(status);
    trigger();
    finish();
}

RS_ActionInterface* RS_ActionSelectInvert::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionSelectInvert(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionSelectInvert::trigger() {
    RS_Selection s(*container, graphicView);
    s.invertSelection();

    RS_DIALOGFACTORY->updateSelectionWidget(container->countSelected());
}


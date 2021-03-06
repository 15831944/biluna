/****************************************************************************
** $Id: rs_actioneditundo.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actioneditundo.h"
#include "rs_snapper.h"
//Added by qt3to4:
#include <QPixmap>

/**
 * Constructor.
 *
 * @param undo true for undo and false for redo.
 */
RS_ActionEditUndo::RS_ActionEditUndo(RS_EntityContainer& container,
                                     RS_GraphicView& graphicView,
                                     bool undo)
        :RS_ActionInterface(container, graphicView) {

    this->undo = undo;
}



RS_ActionEditUndo::~RS_ActionEditUndo() {}


RS_GuiAction* RS_ActionEditUndo::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionEditUndo::getName(), 
            "undo", 
            tr("&Undo"));
        action->setStatusTip(tr("Undoes last action"));
        action->setCommand("undo,oops,oo,u");
        action->setKeycode("oo");
        action->setShortcut("Ctrl+Z");
        action->setFactory(RS_ActionEditUndo::factory);
    }

    return action;
}


RS_ActionInterface* RS_ActionEditUndo::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a =
            new RS_ActionEditUndo(*container, *graphicView, true);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}

void RS_ActionEditUndo::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}



void RS_ActionEditUndo::trigger() {
    if (undo) {
        document->undo();
    } else {
        document->redo();
    }

    document->updateInserts();

    graphicView->redraw();

    finish();
    RS_DIALOGFACTORY->updateSelectionWidget(container->countSelected());
}

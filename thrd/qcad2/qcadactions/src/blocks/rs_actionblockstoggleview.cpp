/****************************************************************************
** $Id: rs_actionblockstoggleview.cpp 9172 2008-02-23 15:25:20Z andrew $
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

#include "rs_actionblockstoggleview.h"

#include "rs_graphic.h"



RS_ActionBlocksToggleView::RS_ActionBlocksToggleView(
    RS_EntityContainer& container,
    RS_GraphicView& graphicView)
        :RS_ActionInterface(container, graphicView) {}


RS_GuiAction* RS_ActionBlocksToggleView::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionBlocksToggleView::getName(), 
            "eyetoggle", 
            tr("&Toggle Visibility"));
        action->setStatusTip(tr("Toggles the visibility of the active block"));
        action->setCommand("bv");
        action->setKeycode("bv");
        action->setFactory(RS_ActionBlocksToggleView::factory);
    }

    return action;
}


void RS_ActionBlocksToggleView::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}


RS_ActionInterface* RS_ActionBlocksToggleView::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionBlocksToggleView(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}


void RS_ActionBlocksToggleView::trigger() {
    RS_DEBUG->print("RS_ActionBlocksToggleView::trigger");
    if (graphic!=NULL) {
        RS_Block* block = graphic->getActiveBlock();
        graphic->toggleBlock(block);
    }
    graphicView->redraw();
    finish();
    RS_DEBUG->print("RS_ActionBlocksToggleView::trigger: OK");
}


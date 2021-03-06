/****************************************************************************
** $Id: rs_actionmodifyexplodetext.cpp 10309 2008-04-05 13:31:40Z andrew $
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

#include "rs_actionmodifyexplodetext.h"

#include "rs_modification.h"
#include "rs_actionselect.h"

/**
 * Constructor.
 */
RS_ActionModifyExplodeText::RS_ActionModifyExplodeText(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_ActionBase(container, graphicView) {
}



RS_ActionModifyExplodeText::~RS_ActionModifyExplodeText() {}


RS_GuiAction* RS_ActionModifyExplodeText::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionModifyExplodeText::getName(),
            "modifyexplodetext",
            tr("&Explode Text into Letters"));
        action->setStatusTip(tr("Explodes text entities into single letters"));
        action->setFactory(RS_ActionModifyExplodeText::factory);
        action->setCommand("explodetext,xe");
        action->setKeycode("xe");
    }

    return action;
}


RS_ActionInterface* RS_ActionModifyExplodeText::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a;
        if (container->countSelected()==0) {
            a = new RS_ActionSelect(*container, *graphicView, 
                RS_ActionModifyExplodeText::factory);
        }
        else {
            a = new RS_ActionModifyExplodeText(*container, *graphicView);
        }
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}


void RS_ActionModifyExplodeText::init(int status) {
    RS_ActionBase::init(status);

    trigger();
    finish();
}



void RS_ActionModifyExplodeText::trigger() {
    RS_Modification m(*container, graphicView);
    m.explodeTextIntoLetters();
}


void RS_ActionModifyExplodeText::updateToolBar() {
    switch (getStatus()) {
    default:
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarModify);
        break;
    }
}

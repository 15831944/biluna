/****************************************************************************
** $Id: rs_actionsetrelativezero.cpp 10309 2008-04-05 13:31:40Z andrew $
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

#include "rs_actionsetrelativezero.h"

#include "rs_snapper.h"
#include "rs_point.h"



RS_ActionSetRelativeZero::RS_ActionSetRelativeZero(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_ActionBase(container, graphicView) {}


RS_GuiAction* RS_ActionSetRelativeZero::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionSetRelativeZero::getName(),  
                "relzeromove", tr("&Set Relative Zero"));
        action->setStatusTip(tr("Set position of the relative zero point"));
        action->setCommand("relativezero,relz,rz");
        action->setKeycode("rz");
        action->setFactory(RS_ActionSetRelativeZero::factory);
    }                                   
    return action;
}


RS_ActionInterface* RS_ActionSetRelativeZero::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionSetRelativeZero(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}


void RS_ActionSetRelativeZero::trigger() {
    if (pt.valid) {
        bool wasLocked = graphicView->isRelativeZeroLocked();
        graphicView->lockRelativeZero(false);
        graphicView->moveRelativeZero(pt);
        graphicView->lockRelativeZero(wasLocked);
    }
    finish();
}



void RS_ActionSetRelativeZero::mouseMoveEvent(RS_MouseEvent* e) {
    clearPreview();
    snapPoint(e);
    drawPreview();
}



void RS_ActionSetRelativeZero::mouseReleaseEvent(RS_MouseEvent* e) {
    if (RS2::qtToRsButtonState(e->button())==RS2::RightButton) {
        clearPreview();
        graphicView->restore();
        init(getStatus()-1);
    } else {
        RS_CoordinateEvent ce(snapPoint(e));
        if (ce.isValid()) {
            coordinateEvent(&ce);
        }
    }
}



void RS_ActionSetRelativeZero::coordinateEvent(RS_CoordinateEvent* e) {
    if (e==NULL) {
        return;
    }

    pt = e->getCoordinate();
    trigger();
}



void RS_ActionSetRelativeZero::updateMouseButtonHints() {
    switch (getStatus()) {
    case 0:
        RS_DIALOGFACTORY->updateMouseWidget(tr("Set relative zero"), tr("Cancel"));
        break;
    default:
        RS_DIALOGFACTORY->updateMouseWidget("", "");
        break;
    }
}



void RS_ActionSetRelativeZero::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}



void RS_ActionSetRelativeZero::updateToolBar() {
    if (!isFinished()) {
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarSnap);
    } else {
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarSnap);
    }
}


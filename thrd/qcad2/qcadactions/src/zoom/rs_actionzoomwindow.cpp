/****************************************************************************
** $Id: rs_actionzoomwindow.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actionzoomwindow.h"

#include "rs.h"
#include "rs_snapper.h"
#include "rs_point.h"
//Added by qt3to4:
#include <QPixmap>


/**
 * Default constructor.
 *
 * @param keepAspectRatio Keep the aspect ratio. true: the factors 
 *          in x and y will stay the same. false Exactly the chosen 
 *          area will be fit to the viewport.
 */
RS_ActionZoomWindow::RS_ActionZoomWindow(RS_EntityContainer& container,
        RS_GraphicView& graphicView, bool keepAspectRatio)
        : RS_ActionInterface(
                            container, graphicView) {

    this->keepAspectRatio = keepAspectRatio;
}


RS_GuiAction* RS_ActionZoomWindow::createGuiAction() {
    
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionZoomWindow::getName(), 
            "zoomwindow",
                                  tr("&Window Zoom"));
        action->setStatusTip(tr("Zooms in a window"));
        action->setCommand("zoomwindow,zw");
        action->setKeycode("zw");
        action->setFactory(RS_ActionZoomWindow::factory);
    }

    return action;
}


void RS_ActionZoomWindow::init(int status) {
    RS_DEBUG->print("RS_ActionZoomWindow::init()");

    RS_ActionInterface::init(status);
    v1 = v2 = RS_Vector(false);
    snapMode = RS2::ActionSnapFree;
    snapRes = RS2::ActionRestrictNothing;
}



RS_ActionInterface* RS_ActionZoomWindow::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionZoomWindow(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionZoomWindow::trigger() {
    RS_DEBUG->print("RS_ActionZoomWindow::trigger()");

    RS_ActionInterface::trigger();

    if (v1.valid && v2.valid) {
        if (graphicView->toGuiDX(v1.distanceTo(v2))>5) {
            graphicView->zoomWindow(v1, v2, keepAspectRatio);
            init();
        }
    }
}



void RS_ActionZoomWindow::mouseMoveEvent(RS_MouseEvent* e) {
    if (getStatus()==1 && v1.valid) {
        v2 = snapPoint(e);
        clearPreview();

        if (v2.valid) {
            preview->addRectangle(v1, v2);
        }
        
        drawPreview();
    }
}



void RS_ActionZoomWindow::mousePressEvent(RS_MouseEvent* e) {
    if (RS2::qtToRsButtonState(e->button())==RS2::LeftButton) {
        switch (getStatus()) {
        case 0:
            v1 = snapPoint(e);
            if (v1.valid) {
                setStatus(1);
            }
            break;

        default:
            break;
        }
    }

    RS_DEBUG->print("RS_ActionZoomWindow::mousePressEvent(): %f %f",
                    v1.x, v1.y);
}



void RS_ActionZoomWindow::mouseReleaseEvent(RS_MouseEvent* e) {
    RS_DEBUG->print("RS_ActionZoomWindow::mouseReleaseEvent()");

    if (RS2::qtToRsButtonState(e->button())==RS2::RightButton) {
        if (getStatus()==1) {
            clearPreview();
            graphicView->restore();
        }
        init(getStatus()-1);
    } else if (RS2::qtToRsButtonState(e->button())==RS2::LeftButton) {
        if (getStatus()==1) {
            v2 = snapPoint(e);
            if (v2.valid) {
                trigger();
            }
        }
    }
}



void RS_ActionZoomWindow::updateMouseButtonHints() {
    RS_DEBUG->print("RS_ActionZoomWindow::updateMouseButtonHints()");

    switch (getStatus()) {
    case 0:
        RS_DIALOGFACTORY->updateMouseWidget(tr("Specify first edge"), tr("Cancel"));
        break;
    case 1:
        RS_DIALOGFACTORY->updateMouseWidget(tr("Specify second edge"), tr("Back"));
        break;
    default:
        RS_DIALOGFACTORY->updateMouseWidget("", "");
        break;
    }
}



void RS_ActionZoomWindow::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::MagnifierCursor);
}


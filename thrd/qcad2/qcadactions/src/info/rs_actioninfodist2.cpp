/****************************************************************************
** $Id: rs_actioninfodist2.cpp 10309 2008-04-05 13:31:40Z andrew $
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

#include "rs_actioninfodist2.h"

#include "rs_snapper.h"



RS_ActionInfoDist2::RS_ActionInfoDist2(RS_EntityContainer& container,
                                       RS_GraphicView& graphicView)
        :RS_ActionBase(container, graphicView) {}


RS_GuiAction* RS_ActionInfoDist2::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionInfoDist2::getName(), 
            "infodist2",
            tr("&Distance Entity to Point"));
        action->setStatusTip(tr("Measures the distance between an "
                            "entity and a point"));
        action->setCommand("infodist2,distance2,ie");
        action->setKeycode("ie");
        action->setFactory(RS_ActionInfoDist2::factory);
    }                                   
    return action;
}

void RS_ActionInfoDist2::init(int status) {
    RS_ActionBase::init(status);

    entity=NULL;
}



RS_ActionInterface* RS_ActionInfoDist2::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionInfoDist2(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionInfoDist2::trigger() {

    RS_DEBUG->print("RS_ActionInfoDist2::trigger()");

    if (point.valid && entity!=NULL) {
        double dist = entity->getDistanceToPoint(point);
        RS_String str;
        str.sprintf("%.6f", dist);
        RS_DIALOGFACTORY->commandMessage(tr("Distance: %1").arg(str));
    }
}



void RS_ActionInfoDist2::mouseMoveEvent(RS_MouseEvent* e) {
    RS_DEBUG->print("RS_ActionInfoDist2::mouseMoveEvent begin");

    switch (getStatus()) {
    case SetEntity:
        RS_DEBUG->print("RS_ActionInfoDist2::mouseMoveEvent SetEntity");
        if (entity!=NULL && point.valid) {
            clearPreview();
            RS_Vector p1 = entity->getNearestPointOnEntity(point);
            preview->addMeasuringLine(p1, point, *graphicView);
            drawPreview();
        }
        break;
        
    case SetPoint:
        RS_DEBUG->print("RS_ActionInfoDist2::mouseMoveEvent SetPoint");
        if (entity!=NULL) {
            clearPreview();
            point = snapPoint(e);
            if (point.valid) {
                RS_Vector p1 = entity->getNearestPointOnEntity(point);
                preview->addMeasuringLine(p1, point, *graphicView);
            }
            drawPreview();
        }
        break;

    default:
        break;
    }

    RS_DEBUG->print("RS_ActionInfoDist2::mouseMoveEvent end");
}



void RS_ActionInfoDist2::mouseReleaseEvent(RS_MouseEvent* e) {
    RS_DEBUG->print("RS_ActionInfoDist2::mouseReleaseEvent");
    if (RS2::qtToRsButtonState(e->button())==RS2::LeftButton) {

        switch (getStatus()) {
        case SetEntity:
            entity = catchEntity(e);
            if (entity!=NULL) {
                setStatus(SetPoint);
            }
            break;

        case SetPoint: {
                RS_CoordinateEvent ce(snapPoint(e));
                if (ce.isValid()) {
                    coordinateEvent(&ce);
                }
            }
            break;

        default:
            break;
        }
    } else if (RS2::qtToRsButtonState(e->button())==RS2::RightButton) {
        clearPreview();
        graphicView->restore();
        init(getStatus()-1);
    }
    RS_DEBUG->print("RS_ActionInfoDist2::mouseReleaseEvent: OK");
}


void RS_ActionInfoDist2::coordinateEvent(RS_CoordinateEvent* e) {
    if (e==NULL) {
        return;
    }
    RS_DEBUG->print("RS_ActionInfoDist2::coordinateEvent");

    if (getStatus()==SetPoint && entity!=NULL) {
        point = e->getCoordinate();
        graphicView->moveRelativeZero(point);
        graphicView->restore();
        trigger();
        setStatus(SetEntity);
    }
    RS_DEBUG->print("RS_ActionInfoDist2::coordinateEvent: OK");
}



void RS_ActionInfoDist2::updateMouseButtonHints() {
    switch (getStatus()) {
    case SetEntity:
        RS_DIALOGFACTORY->updateMouseWidget(
            tr("Specify entity"),
            tr("Cancel"));
        break;
    case SetPoint:
        RS_DIALOGFACTORY->updateMouseWidget(
            tr("Specify point"),
            tr("Back"));
        break;
    default:
        RS_DIALOGFACTORY->updateMouseWidget("", "");
        break;
    }
}



void RS_ActionInfoDist2::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}



void RS_ActionInfoDist2::updateToolBar() {
    switch (getStatus()) {
    case SetPoint:
    case SetEntity:
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarSnap);
        break;
    default:
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarInfo);
        break;
    }
}



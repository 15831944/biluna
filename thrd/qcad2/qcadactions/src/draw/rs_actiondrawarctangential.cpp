/****************************************************************************
** $Id: rs_actiondrawarctangential.cpp 9299 2008-03-03 14:12:57Z andrew $
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

#include "rs_actiondrawarctangential.h"

#include "rs_snapper.h"
#include "rs_dialogfactory.h"



RS_ActionDrawArcTangential::RS_ActionDrawArcTangential(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_ActionBase(container, graphicView) {
    reset();
}



RS_ActionDrawArcTangential::~RS_ActionDrawArcTangential() {}


RS_GuiAction* RS_ActionDrawArcTangential::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionDrawArcTangential::getName(), 
            "arcstangential", 
            tr("&Tangential"));
        action->setStatusTip(tr("Draw arcs tangential to base entity"));
        action->setCommand("arctangential");
        action->setKeycode("an");
        action->setFactory(RS_ActionDrawArcTangential::factory);
    }                                  
    return action;
}


void RS_ActionDrawArcTangential::reset() {
    baseEntity = NULL;
    isStartPoint = false;
    point = RS_Vector(false);
}



void RS_ActionDrawArcTangential::init(int status) {
    RS_ActionBase::init(status);

    //reset();
}



RS_ActionInterface* RS_ActionDrawArcTangential::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionDrawArcTangential(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionDrawArcTangential::trigger() {
    RS_ActionBase::trigger();

    if (point.valid==false || baseEntity==NULL) {
        RS_DEBUG->print("RS_ActionDrawArcTangential::trigger: "
                        "conditions not met");
        return;
    }

    preparePreview();
    RS_Arc* arc = new RS_Arc(container, data);
    arc->setLayerToActive();
    arc->setPenToActive();
    container->addEntity(arc);

    // upd. undo list:
    if (document!=NULL) {
        document->startUndoCycle();
        document->addUndoable(arc);
        document->endUndoCycle();
    }

    clearPreview();
    graphicView->createPainter();
    graphicView->drawEntity(arc);
    graphicView->moveRelativeZero(arc->getEndpoint());
    graphicView->restore();

    setStatus(SetBaseEntity);
    reset();
}



void RS_ActionDrawArcTangential::preparePreview() {
    if (baseEntity!=NULL && point.valid) {
        RS_Vector startPoint;
        double direction;
        if (isStartPoint) {
            startPoint = baseEntity->getStartpoint();
            direction = RS_Math::correctAngle(baseEntity->getDirection1()+M_PI);
        } else {
            startPoint = baseEntity->getEndpoint();
            direction = RS_Math::correctAngle(baseEntity->getDirection2()+M_PI);
        }

        RS_Arc arc(NULL, RS_ArcData());
        bool suc = arc.createFrom2PDirectionRadius(startPoint, point, direction, data.radius);
        if (suc) {
            data = arc.getData();
        }
    }
}


void RS_ActionDrawArcTangential::mouseMoveEvent(RS_MouseEvent* e) {
    clearPreview();
    
    switch (getStatus()) {
    case SetBaseEntity:
        break;

    case SetEndAngle: {
            point = snapPoint(e);
            if (point.valid) {
                preparePreview();
                if (data.isValid()) {
                    RS_Arc* arc = new RS_Arc(preview, data);
                    preview->addEntity(arc);

                    RS_Vector p1;
                    p1.setPolar(data.radius, data.angle1);
                    RS_Line* line1 = new RS_Line(
                        preview, 
                        RS_LineData(
                            data.center,
                            data.center + p1
                        )
                    );
                    line1->setPen(RS_Preview::getPreviewPen());
                    RS_Line* line2 = new RS_Line(
                        preview, 
                        RS_LineData(
                            data.center,
                            point
                        )
                    );
                    line2->setPen(RS_Preview::getPreviewPen());
                    preview->addEntity(line1);
                    preview->addEntity(line2);

                    graphicView->moveRelativeZero(data.center);
                    graphicView->restore();
                }
            }
        }
        break;

    default:
        break;
    }
    drawPreview();
}



void RS_ActionDrawArcTangential::mouseReleaseEvent(RS_MouseEvent* e) {
    if (RS2::qtToRsButtonState(e->button())==RS2::LeftButton) {
        switch (getStatus()) {

            // set base entity:
        case SetBaseEntity: {
                RS_Vector coord = graphicView->toGraph(e->x(), e->y());
                RS_Entity* entity = catchEntity(coord, RS2::ResolveAll);
                if (entity!=NULL) {
                    if (entity->isAtomic()) {
                        baseEntity = dynamic_cast<RS_AtomicEntity*>(entity);
                        if (baseEntity->getStartpoint().distanceTo(coord) <
                                baseEntity->getEndpoint().distanceTo(coord)) {
                            isStartPoint = true;
                        } else {
                            isStartPoint = false;
                        }
                        setStatus(SetEndAngle);
                        updateMouseButtonHints();
                    } else {
                        // TODO: warning
                    }
                }
                //else {
                //    deleteSnapper();
                //}
            }
            break;

            // set angle (point that defines the angle)
        case SetEndAngle: {
                RS_CoordinateEvent ce(snapPoint(e));
                if (ce.isValid()) {
                    coordinateEvent(&ce);
                }
            }
            break;
        }
    } else if (RS2::qtToRsButtonState(e->button())==RS2::RightButton) {
        clearPreview();
        graphicView->restore();
        init(getStatus()-1);
    }
}



void RS_ActionDrawArcTangential::coordinateEvent(RS_CoordinateEvent* e) {
    if (e==NULL) {
        return;
    }
    RS_Vector mouse = e->getCoordinate();

    switch (getStatus()) {
    case SetBaseEntity:
        break;

    case SetEndAngle:
        point = mouse;
        trigger();
        break;

    default:
        break;
    }
}



void RS_ActionDrawArcTangential::commandEvent(RS_CommandEvent* e) {
    RS_String c = e->getCommand().toLower();

    if (checkCommand("help", c)) {
        RS_DIALOGFACTORY->commandMessage(msgAvailableCommands()
                                         + getAvailableCommands().join(", "));
        return;
    }
}



RS_StringList RS_ActionDrawArcTangential::getAvailableCommands() {
    RS_StringList cmd = RS_ActionBase::getAvailableCommands();
    return cmd;
}


/*
void RS_ActionDrawArcTangential::showOptions() {
    RS_ActionBase::showOptions();

    if (RS_DIALOGFACTORY!=NULL) {
        RS_DIALOGFACTORY->requestOptions(this, true);
    }
    updateMouseButtonHints();
}
*/



void RS_ActionDrawArcTangential::updateMouseButtonHints() {
    switch (getStatus()) {
    case SetBaseEntity:
        RS_DIALOGFACTORY->updateMouseWidget(
            tr("Specify base entity"),
            tr("Cancel"));
        break;
    case SetEndAngle:
        RS_DIALOGFACTORY->updateMouseWidget(
            tr("Specify end angle"), tr("Back"));
        break;
    default:
        RS_DIALOGFACTORY->updateMouseWidget("", "");
        break;
    }
}



void RS_ActionDrawArcTangential::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}



void RS_ActionDrawArcTangential::updateToolBar() {
    if (!isFinished()) {
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarSnap);
    } else {
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarArcs);
    }
}


/****************************************************************************
** $Id: rs_actiondrawlinetangent2.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actiondrawlinetangent2.h"

#include "rs_creation.h"
#include "rs_snapper.h"



RS_ActionDrawLineTangent2::RS_ActionDrawLineTangent2(
    RS_EntityContainer& container,
    RS_GraphicView& graphicView)
        :RS_ActionBase(container, graphicView) {

    tangent = NULL;
    circle1 = NULL;
    circle2 = NULL;
}

RS_GuiAction* RS_ActionDrawLineTangent2::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionDrawLineTangent2::getName(), 
            "linestan2",
                tr("Tan&gent (C,C)"));
        action->setStatusTip(tr("Draw tangent (circle, circle)"));
        action->setCommand("linetangent2,tangent2");
        action->setKeycode("lt2");
        action->setFactory(RS_ActionDrawLineTangent2::factory);
    }                                   

    return action;
}


RS_ActionInterface* RS_ActionDrawLineTangent2::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionDrawLineTangent2(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionDrawLineTangent2::trigger() {
    RS_ActionBase::trigger();

    if (tangent!=NULL) {
        RS_Entity* newEntity = NULL;

        newEntity = new RS_Line(container,
                                tangent->getData());

        if (newEntity!=NULL) {
            newEntity->setLayerToActive();
            newEntity->setPenToActive();
            container->addEntity(newEntity);

            // upd. undo list:
            if (document!=NULL) {
                document->startUndoCycle();
                document->addUndoable(newEntity);
                document->endUndoCycle();
            }
            graphicView->createPainter();
            graphicView->drawEntity(newEntity);
            graphicView->restore();
            setStatus(SetCircle1);
        }
        delete tangent;
        tangent = NULL;
    } else {
        RS_DEBUG->print("RS_ActionDrawLineTangent2::trigger:"
                        " Entity is NULL\n");
    }
}



void RS_ActionDrawLineTangent2::mouseMoveEvent(RS_MouseEvent* e) {
    RS_DEBUG->print("RS_ActionDrawLineTangent2::mouseMoveEvent begin");

    RS_Vector mouse(graphicView->toGraphX(e->x()),
                    graphicView->toGraphY(e->y()));
    clearPreview();

    switch (getStatus()) {
    case SetCircle1: {
            RS_Entity* en = catchEntity(e, RS2::ResolveAll);
            if (en!=NULL && (en->rtti()==RS2::EntityCircle ||
                             en->rtti()==RS2::EntityArc)) {
                circle1 = en;
            }
        }
        break;

    case SetCircle2: {
            RS_Entity* en = catchEntity(e, RS2::ResolveAll);
            if (en!=NULL && (en->rtti()==RS2::EntityCircle ||
                             en->rtti()==RS2::EntityArc)) {
                circle2 = en;

                RS_Creation creation(NULL, NULL);
                RS_Line* t = creation.createTangent2(mouse,
                                                     circle1,
                                                     circle2);

                if (t!=NULL) {
                    if (tangent!=NULL) {
                        delete tangent;
                    }
                    tangent = dynamic_cast<RS_Line*>(t->clone());

                    preview->addEntity(t);
                }
            }
        }
        break;

    default:
        break;
    }

    drawPreview();

    RS_DEBUG->print("RS_ActionDrawLineTangent2::mouseMoveEvent end");
}



void RS_ActionDrawLineTangent2::mouseReleaseEvent(RS_MouseEvent* e) {

    if (RS2::qtToRsButtonState(e->button())==RS2::RightButton) {
        deletePreview();
        clearPreview();
        init(getStatus()-1);
    } else {
        switch (getStatus()) {
        case SetCircle1:
            setStatus(SetCircle2);
            break;

        case SetCircle2:
            trigger();
            break;
        }
    }

}



void RS_ActionDrawLineTangent2::updateMouseButtonHints() {
    if (RS_DIALOGFACTORY!=NULL) {
        switch (getStatus()) {
        case SetCircle1:
            RS_DIALOGFACTORY->updateMouseWidget(tr("Select first circle or arc"),
                                                tr("Cancel"));
            break;
        case SetCircle2:
            RS_DIALOGFACTORY->updateMouseWidget(tr("Select second circle or arc"),
                                                tr("Back"));
            break;
        default:
            RS_DIALOGFACTORY->updateMouseWidget("", "");
            break;
        }
    }
}



void RS_ActionDrawLineTangent2::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}



void RS_ActionDrawLineTangent2::updateToolBar() {
    if (RS_DIALOGFACTORY!=NULL) {
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarLines);
    }
}


/****************************************************************************
** $Id: rs_actionmodifytrim.cpp 10309 2008-04-05 13:31:40Z andrew $
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

#include "rs_actionmodifytrim.h"

#include "rs_snapper.h"


RS_GuiAction* RS_ActionModifyTrim::action = NULL;

/**
 * @param both Trim both entities.
 */
RS_ActionModifyTrim::RS_ActionModifyTrim(RS_EntityContainer& container,
        RS_GraphicView& graphicView, bool both)
        :RS_ActionBase(container, graphicView) {

    trimEntity = NULL;
    trimCoord = RS_Vector(false);
    limitEntity = NULL;
    limitCoord = RS_Vector(false);
    this->both = both;
}

RS_GuiAction* RS_ActionModifyTrim::createGuiAction() {

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionModifyTrim::getName(), 
            "modifytrim",
            tr("&Trim"));
        action->setStatusTip(tr("Trim or extend entities to meet other entities"));
        action->setCommand("trim,extend,rm,xt,mr");
        action->setKeycode("rm,xt");
        action->setFactory(RS_ActionModifyTrim::factory);
#ifdef RS_ARCH
        action->setCheckable(true);
#endif
    }

    return action;
}

RS_ActionInterface* RS_ActionModifyTrim::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* action = 
            new RS_ActionModifyTrim(*container, *graphicView, false);
        graphicView->setCurrentAction(action);
        return action;
    }

    return NULL;

}



void RS_ActionModifyTrim::finish() {
    RS_ActionBase::finish();

#ifdef RS_ARCH
    action->setChecked(false);
#endif
}


void RS_ActionModifyTrim::init(int status) {
    RS_ActionBase::init(status);

    snapMode = RS2::ActionSnapFree;
    snapRes = RS2::ActionRestrictNothing;
}



void RS_ActionModifyTrim::trigger() {

    RS_DEBUG->print("RS_ActionModifyTrim::trigger()");

    if (trimEntity!=NULL && trimEntity->isAtomic() &&
            limitEntity!=NULL) {

        RS_Modification m(*container, graphicView);
        m.trim(trimCoord, dynamic_cast<RS_AtomicEntity*>(trimEntity),
               limitCoord, limitEntity,
               both);

        trimEntity = NULL;
        if (both) {
            setStatus(ChooseLimitEntity);
        } else {
            setStatus(ChooseTrimEntity);
        }

        RS_DIALOGFACTORY->updateSelectionWidget(container->countSelected());
    }
}



void RS_ActionModifyTrim::mouseMoveEvent(RS_MouseEvent* e) {
    RS_DEBUG->print("RS_ActionModifyTrim::mouseMoveEvent");
            
    RS_Vector mouse = graphicView->toGraph(e->x(), e->y());

    switch (getStatus()) {
    case ChooseLimitEntity: {
        limitCoord = mouse;
        limitEntity = catchEntity(e);
        } break;

    case ChooseTrimEntity:
        if (limitEntity!=NULL) {

            clearPreview();

            RS_Entity* clone = limitEntity->clone();
            clone->setUpdateEnabled(false);
            clone->setHighlighted(true);
            clone->reparent(preview);
            preview->addPreviewEntity(clone);

            trimCoord = mouse;
            trimEntity = catchEntity(e);

            drawPreview();
        }
        break;

    default:
        break;
    }

    RS_DEBUG->print("RS_ActionModifyTrim::mouseMoveEvent: OK");
}



void RS_ActionModifyTrim::mouseReleaseEvent(RS_MouseEvent* e) {
    RS_DEBUG->print("RS_ActionModifyTrim::mouseReleaseEvent");

    if (RS2::qtToRsButtonState(e->button())==RS2::LeftButton) {

        RS_Vector mouse = graphicView->toGraph(e->x(), e->y());

        switch (getStatus()) {
        case ChooseLimitEntity:
            limitCoord = mouse;
            limitEntity = catchEntity(e);
            if (limitEntity!=NULL) {
                setStatus(ChooseTrimEntity);
            }
            break;

        case ChooseTrimEntity:
            RS_DEBUG->print("RS_ActionModifyTrim::mouseReleaseEvent:"
                "choose trim entity");
            trimEntity = catchEntity(e);
            trimCoord = mouse;
            if (trimEntity!=NULL && trimEntity->isAtomic()) {
                trigger();
            }
            break;

        default:
            break;
        }
    } else if (RS2::qtToRsButtonState(e->button())==RS2::RightButton) {
        clearPreview();
        /*
        if (limitEntity!=NULL) {
            limitEntity->setHighlighted(false);
            graphicView->createPainter();
            graphicView->drawEntity(limitEntity);
        }
        */
        graphicView->restore();
        init(getStatus()-1);
    }
    
    RS_DEBUG->print("RS_ActionModifyTrim::mouseReleaseEvent: OK");
}



void RS_ActionModifyTrim::updateMouseButtonHints() {
    switch (getStatus()) {
    case ChooseLimitEntity:
        if (both) {
            RS_DIALOGFACTORY->updateMouseWidget(tr("Select first trim entity"),
                                                tr("Cancel"));
        } else {
            RS_DIALOGFACTORY->updateMouseWidget(tr("Select limiting entity"),
                                                tr("Back"));
        }
        break;
    case ChooseTrimEntity:
        if (both) {
            RS_DIALOGFACTORY->updateMouseWidget(tr("Select second trim entity"),
                                                tr("Cancel"));
        } else {
            RS_DIALOGFACTORY->updateMouseWidget(tr("Select entity to trim"),
                                                tr("Back"));
        }
        break;
    default:
        RS_DIALOGFACTORY->updateMouseWidget("", "");
        break;
    }
}



void RS_ActionModifyTrim::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}



void RS_ActionModifyTrim::updateToolBar() {
    RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarModify);
}



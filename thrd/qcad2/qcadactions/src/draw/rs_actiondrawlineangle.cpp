/****************************************************************************
** $Id: rs_actiondrawlineangle.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actiondrawlineangle.h"
#include "rs_snapper.h"



RS_ActionDrawLineAngle::RS_ActionDrawLineAngle(RS_EntityContainer& container,
        RS_GraphicView& graphicView,
        double angle, bool fixedAngle)
        :RS_ActionBase(
                           container, graphicView) {

    this->angle = angle;
    length = 1.0;
    snapReference = 0;
    this->fixedAngle = fixedAngle;
    pos = RS_Vector(false);
    reset();
}



RS_ActionDrawLineAngle::~RS_ActionDrawLineAngle() {}


RS_GuiAction* RS_ActionDrawLineAngle::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionDrawLineAngle::getName(),
            "linesangle",
            tr("&Angle"));
        action->setStatusTip(tr("Draw lines with a given angle"));
        action->setCommand("lineangle,la");
        action->setKeycode("la");
        action->setFactory(RS_ActionDrawLineAngle::factory);
    }
    
    return action;
}

void RS_ActionDrawLineAngle::reset() {
    data = RS_LineData(RS_Vector(false),
                       RS_Vector(false));
}



void RS_ActionDrawLineAngle::init(int status) {
    RS_ActionBase::init(status);

    reset();
}



RS_ActionInterface* RS_ActionDrawLineAngle::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionDrawLineAngle(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionDrawLineAngle::trigger() {
    RS_ActionBase::trigger();

    preparePreview();
    RS_Line* line = new RS_Line(container,
                                data);
    line->setLayerToActive();
    line->setPenToActive();
    container->addEntity(line);

    // upd. undo list:
    if (document!=NULL) {
        document->startUndoCycle();
        document->addUndoable(line);
        document->endUndoCycle();
    }
    graphicView->createPainter();
    graphicView->drawEntity(line);
    graphicView->moveRelativeZero(data.startpoint);
    graphicView->restore();
    RS_DEBUG->print("RS_ActionDrawLineAngle::trigger(): line added: %d",
                    line->getId());
}



void RS_ActionDrawLineAngle::mouseMoveEvent(RS_MouseEvent* e) {
    RS_DEBUG->print("RS_ActionDrawLineAngle::mouseMoveEvent begin");
    clearPreview();

    if (getStatus()==SetPos) {
        pos = snapPoint(e);
        if (pos.valid) {
            preparePreview();
            preview->addEntity(new RS_Line(preview,
                                           data));
        }
    }
    drawPreview();

    RS_DEBUG->print("RS_ActionDrawLineAngle::mouseMoveEvent end");
}



void RS_ActionDrawLineAngle::mouseReleaseEvent(RS_MouseEvent* e) {
    if (RS2::qtToRsButtonState(e->button())==RS2::LeftButton) {
        if (getStatus()==SetPos) {
            RS_CoordinateEvent ce(snapPoint(e));
            if (ce.isValid()) {
                coordinateEvent(&ce);
            }
        }
    } else if (RS2::qtToRsButtonState(e->button())==RS2::RightButton) {
        clearPreview();
        graphicView->restore();
        init(getStatus()-1);
    }
}


void RS_ActionDrawLineAngle::preparePreview() {
    RS_Vector p1, p2;
    // End:
    if (snapReference == 2) {
        p2.setPolar(length * -1, angle);
    } else {
        p2.setPolar(length, angle);
    }

    // Middle:
    if (snapReference == 1) {
        p1 = pos - (p2 / 2);
    } else {
        p1 = pos;
    }

    p2 += p1;
    data = RS_LineData(p1, p2);
}


void RS_ActionDrawLineAngle::coordinateEvent(RS_CoordinateEvent* e) {
    if (e==NULL) {
        return;
    }

    switch (getStatus()) {
    case SetPos:
        pos = e->getCoordinate();
        trigger();
        break;

    default:
        break;
    }
}



void RS_ActionDrawLineAngle::commandEvent(RS_CommandEvent* e) {
    RS_String c = e->getCommand().toLower();

    if (checkCommand("help", c)) {
        RS_DIALOGFACTORY->commandMessage(msgAvailableCommands()
                                         + getAvailableCommands().join(", "));
        return;
    }

    switch (getStatus()) {
    case SetPos:
        if (!fixedAngle && checkCommand("angle", c)) {
            clearPreview();
            graphicView->restore();
            setStatus(SetAngle);
        } else if (checkCommand("length", c)) {
            clearPreview();
            graphicView->restore();
            setStatus(SetLength);
        }
        break;

    case SetAngle: {
            bool ok;
            double a = RS_Math::eval(c, &ok);
            if (ok==true) {
                angle = RS_Math::deg2rad(a);
            } else {
                RS_DIALOGFACTORY->commandMessage(tr("Not a valid expression"));
            }
            RS_DIALOGFACTORY->requestOptions(this, true, true);
            setStatus(SetPos);
        }
        break;

    case SetLength: {
            bool ok;
            double l = RS_Math::eval(c, &ok);
            if (ok==true) {
                length = l;
            } else {
                RS_DIALOGFACTORY->commandMessage(tr("Not a valid expression"));
            }
            RS_DIALOGFACTORY->requestOptions(this, true, true);
            setStatus(SetPos);
        }
        break;

    default:
        break;
    }
}



RS_StringList RS_ActionDrawLineAngle::getAvailableCommands() {
    RS_StringList cmd = RS_ActionBase::getAvailableCommands();

    switch (getStatus()) {
    case SetPos:
        if (!fixedAngle) {
            cmd += command("angle");
        }
        cmd += command("length");
        break;
    default:
        break;
    }

    return cmd;
}


void RS_ActionDrawLineAngle::updateMouseButtonHints() {
    switch (getStatus()) {
    case SetPos:
        RS_DIALOGFACTORY->updateMouseWidget(tr("Specify position [angle/length]"),
                                            tr("Cancel"));
        break;

    case SetAngle:
        RS_DIALOGFACTORY->updateMouseWidget(tr("Enter angle:"), tr("Back"));
        break;

    case SetLength:
        RS_DIALOGFACTORY->updateMouseWidget(tr("Enter length:"), tr("Back"));
        break;

    default:
        RS_DIALOGFACTORY->updateMouseWidget("", "");
        break;
    }
}



void RS_ActionDrawLineAngle::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}



void RS_ActionDrawLineAngle::updateToolBar() {
    if (!isFinished()) {
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarSnap);
    } else {
        RS_DIALOGFACTORY->requestToolBar(RS2::ToolBarLines);
    }
}



/*
RS_Widget* RS_ActionDrawLineAngle::createOptionWidget() {
    
}
*/

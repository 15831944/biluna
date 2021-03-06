/****************************************************************************
** $Id: rs_actionselectbase.cpp 8361 2008-01-23 15:20:59Z andrew $
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

#include "rs_actionselectbase.h"



RS_ActionSelectBase::RS_ActionSelectBase(
    RS_EntityContainer& container,
    RS_GraphicView& graphicView)
    : RS_ActionInterface(container, graphicView) {}



/**
 * Default behaviour of this method is triggering the predecesing
 * action and finishing this one when the enter key is pressed.
 */
void RS_ActionSelectBase::keyReleaseEvent(RS_KeyEvent* e) {
    if (e->key()==Qt::Key_Return && predecessor!=NULL) {
        finish();
    }
}



void RS_ActionSelectBase::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::SelectCursor);
}


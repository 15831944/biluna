/****************************************************************************
** $Id: rs_actionzoomselection.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actionzoomselection.h"
//Added by qt3to4:
#include <QPixmap>


/**
 * Constructor.
 */
RS_ActionZoomSelection::RS_ActionZoomSelection(RS_EntityContainer& container,
                                     RS_GraphicView& graphicView)
        :RS_ActionInterface( container, graphicView) {
}


RS_GuiAction* RS_ActionZoomSelection::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionZoomSelection::getName(), 
            "zoomselection",
            tr("Zoom to &Selection"));
        action->setStatusTip(tr("Zooms to current selection"));
        action->setCommand("zoomselection,zs");
        action->setKeycode("zs");
        action->setFactory(RS_ActionZoomSelection::factory);
    }
    return action;
}


void RS_ActionZoomSelection::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}



RS_ActionInterface* RS_ActionZoomSelection::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionZoomSelection(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionZoomSelection::trigger() {
    graphicView->zoomSelection();
    finish();
}


/****************************************************************************
** $Id: rs_actionlayerstoggleview.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actionlayerstoggleview.h"

#include "rs_graphic.h"



RS_ActionLayersToggleView::RS_ActionLayersToggleView(
    RS_EntityContainer& container,
    RS_GraphicView& graphicView)
        :RS_ActionInterface(
                    container, graphicView) {}


RS_GuiAction* RS_ActionLayersToggleView::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionLayersToggleView::getName(), 
            "eyetoggle", 
            tr("Toggle &Visibility"));
        action->setStatusTip(tr("Toggles the visibility of the currently active layer"));
        action->setCommand("yv");
        action->setKeycode("yv");
        action->setFactory(RS_ActionLayersToggleView::factory);
        
    }                                   
    return action;
}

RS_ActionInterface* RS_ActionLayersToggleView::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionLayersToggleView(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionLayersToggleView::trigger() {
    RS_DEBUG->print("RS_ActionLayersToggleView::trigger");

    if (graphic!=NULL) {
        RS_Layer* layer = graphic->getActiveLayer();
        if (layer!=NULL) {
            RS_DEBUG->print("RS_ActionLayersToggleView::trigger: layer: '%s'", 
                (const char*)layer->getName().toLatin1());
        }
        else {
            RS_DEBUG->print("RS_ActionLayersToggleView::trigger: layer is NULL");
        }
        graphic->toggleLayer(layer);
    }

    finish();
}



void RS_ActionLayersToggleView::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}


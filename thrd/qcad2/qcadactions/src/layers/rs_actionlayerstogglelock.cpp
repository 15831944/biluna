/****************************************************************************
** $Id: rs_actionlayerstogglelock.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actionlayerstogglelock.h"

#include "rs_graphic.h"



RS_ActionLayersToggleLock::RS_ActionLayersToggleLock(
    RS_EntityContainer& container,
    RS_GraphicView& graphicView)
        :RS_ActionInterface(
                    container, graphicView) {}


RS_GuiAction* RS_ActionLayersToggleLock::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionLayersToggleLock::getName(), 
            "locktoggle",
            tr("Toggle &Lock"));
        action->setStatusTip(tr("Toggles the lock status of the currently active layer"));
        action->setCommand("yl");
        action->setKeycode("yl");
        action->setFactory(RS_ActionLayersToggleLock::factory);
    }
    return action;
}

RS_ActionInterface* RS_ActionLayersToggleLock::factory() {
    RS_EntityContainer* container = RS_DIALOGFACTORY->getDocument();
    RS_GraphicView* graphicView = RS_DIALOGFACTORY->getGraphicView();
    
    if (container!=NULL && graphicView!=NULL) {
        RS_ActionInterface* a = new RS_ActionLayersToggleLock(*container, *graphicView);
        graphicView->setCurrentAction(a);
        return a;
    }

    return NULL;
}



void RS_ActionLayersToggleLock::trigger() {
    RS_DEBUG->print("toggle layer");
    if (graphic!=NULL) {
        RS_Layer* layer = graphic->getActiveLayer();
        if (layer!=NULL) {
            graphic->toggleLayerLock(layer);

            // deselect entities on locked layer:
            if (layer->isLocked()) {
                for (RS_Entity* e=container->firstEntity(); e!=NULL;
                        e=container->nextEntity()) {
                    if (e!=NULL && e->isVisible() && e->getLayer()==layer) {

                        if (graphicView!=NULL) {
                            graphicView->deleteEntity(e);
                        }

                        e->setSelected(false);

                        if (graphicView!=NULL) {
                            graphicView->drawEntity(e);
                        }
                    }
                }
            }
        }

    }
    finish();
}



void RS_ActionLayersToggleLock::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}

// EOF

/****************************************************************************
** $Id: rs_graphicview.cpp 9538 2008-04-03 10:32:33Z andrew $
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


#include "rs_graphicview.h"

#include <stdio.h>

#include "rs_application.h"
#include "rs_actioninterface.h"
#include "rs_block.h"
#include "rs_eventhandler.h"
#include "rs_entity.h"
#include "rs_graphic.h"
#include "rs_grid.h"
#include "rs_insert.h"
#include "rs_keyevent.h"
#include "rs_layer.h"
#include "rs_line.h"
#include "rs_mouseevent.h"
#include "rs_painter.h"
#include "rs_painterqt.h"
#include "rs_preview.h"
#include "rs_text.h"
#include "rs_settings.h"
#include "rs_solid.h"




/**
 * Constructor.
 */
RS_GraphicView::RS_GraphicView()
        : background(), foreground() {
    drawingMode = RS2::ModeFull;
    printing = false;
    deleteMode = false;
    factor = RS_Vector(1.0,1.0);
    offsetX = 0;
    offsetY = 0;
    previousFactor = RS_Vector(1.0,1.0);
    previousOffsetX = 0;
    previousOffsetY = 0;
    container = NULL;
    eventHandler = new RS_EventHandler(this);
    gridColor = RS_Color(Qt::gray);
    metaGridColor = RS_Color(64,64,64);
    grid = new RS_Grid(this);
    updateEnabled = 0;
    zoomFrozen = false;
    draftMode = false;
    painter = NULL;
    
    preview = new RS_Preview(NULL);

    borderLeft = 0;
    borderTop = 0;
    borderRight = 0;
    borderBottom = 0;

    relativeZero = RS_Vector(false);
    relativeZeroLocked=false;

    mx = my = 0;

    defaultSnapMode = RS2::ActionSnapAuto;
    defaultSnapRes = RS2::ActionRestrictNothing;

    RS_SETTINGS->beginGroup("/Appearance");
    setBackground(QColor(RS_SETTINGS->readEntry("/BackgroundColor", "#000000")));
    setGridColor(QColor(RS_SETTINGS->readEntry("/GridColor", "#7F7F7F")));
    setMetaGridColor(QColor(RS_SETTINGS->readEntry("/MetaGridColor", "#3F3F3F")));
    setSelectedColor(QColor(RS_SETTINGS->readEntry("/SelectedColor", "#A54747")));
    setHighlightedColor(QColor(RS_SETTINGS->readEntry("/HighlightedColor",
                               "#739373")));
    RS_SETTINGS->endGroup();

    printPreview = false;

    simulationSpeed = 100;
    simulationSmooth = false;
    simulationRapid = false;
    simulationRunning = false;
}



/**
 * Destructor.
 */
RS_GraphicView::~RS_GraphicView() {
    RS_DEBUG->print("RS_GraphicView::~RS_GraphicView");
    delete eventHandler;
    if (painter!=NULL) {
        delete painter;
    }
    delete grid;
    delete preview;
    RS_DEBUG->print("RS_GraphicView::~RS_GraphicView: OK");
}



/**
 * Must be called by any derrived class in the destructor.
 */
void RS_GraphicView::cleanUp() {
}



/**
 * Sets the pointer to the graphic which contains the entities
 * which are visualized by this widget.
 */
void RS_GraphicView::setContainer(RS_EntityContainer* container) {
    this->container = container;
    preview->setParentContainer(container);
}



/**
 * Sets the zoom factor in X for this visualization of the graphic.
 */
bool RS_GraphicView::setFactorX(double f) {
    RS_DEBUG->print("RS_GraphicView::setFactorX: %f", f);
    if (!zoomFrozen) {
        f = fabs(f);
        if (checkFactor(f)) {
            factor.x = f;
            return true;
        }
    }

    return false;
}



/**
 * Sets the zoom factor in Y for this visualization of the graphic.
 */
bool RS_GraphicView::setFactorY(double f) {
    if (!zoomFrozen) {
        f = fabs(f);
        if (checkFactor(f)) {
            factor.y = f;
            return true;
        }
    }
    
    return false;
}



/**
 * @return true if the grid is switched on.
 */
bool RS_GraphicView::isGridOn() {
    if (container!=NULL) {
        RS_Graphic* g = container->getGraphic();
        if (g!=NULL) {
            return g->isGridOn();
        }
    }
    return true;
}




/**
 * Centers the drawing in x-direction.
 */
void RS_GraphicView::centerOffsetX() {
    if (container!=NULL && !zoomFrozen) {
        offsetX = (int)(((getWidth()-borderLeft-borderRight)
                         - (container->getSize().x*factor.x))/2.0
                        - (container->getMin().x*factor.x)) + borderLeft;
    }
}



/**
 * Centers the drawing in y-direction.
 */
void RS_GraphicView::centerOffsetY() {
    if (container!=NULL && !zoomFrozen) {
        offsetY = (int)((getHeight()-borderTop-borderBottom
                         - (container->getSize().y*factor.y))/2.0
                        - (container->getMin().y*factor.y)) + borderBottom;
    }
}



/**
 * Centers the given coordinate in the view in x-direction.
 */
void RS_GraphicView::centerX(double v) {
    if (!zoomFrozen) {
        offsetX = (int)((v*factor.x)
                        - (double)(getWidth()-borderLeft-borderRight)/2.0);
    }
}



/**
 * Centers the given coordinate in the view in y-direction.
 */
void RS_GraphicView::centerY(double v) {
    if (!zoomFrozen) {
        offsetY = (int)((v*factor.y)
                        - (double)(getHeight()-borderTop-borderBottom)/2.0);
    }
}



/**
 * Updates the view controls.
 */
/*
void RS_GraphicView::updateView() {
    static int running = 0;
    running++;
    if (running<100) {
        adjustZoomControls();
        adjustOffsetControls();
    }
    running--;
    if(running==0) {
        redraw();
    }
}
*/


/**
 * @return Current action or NULL.
 */
RS_ActionInterface* RS_GraphicView::getDefaultAction() {
    if (eventHandler!=NULL) {
        return eventHandler->getDefaultAction();
    } else {
        return NULL;
    }
}



/**
 * Sets the default action of the event handler.
 */
void RS_GraphicView::setDefaultAction(RS_ActionInterface* action) {
    if (eventHandler!=NULL) {
        eventHandler->setDefaultAction(action);
    }
}



/**
 * @return Current action or NULL.
 */
RS_ActionInterface* RS_GraphicView::getCurrentAction() {
    if (eventHandler!=NULL) {
        return eventHandler->getCurrentAction();
    } else {
        return NULL;
    }
}



/**
 * Sets the current action of the event handler.
 */
void RS_GraphicView::setCurrentAction(RS_ActionInterface* action) {
    RS_DEBUG->print("RS_GraphicView::setCurrentAction");
    if (eventHandler!=NULL) {
        eventHandler->setCurrentAction(action);
    }
    RS_DEBUG->print("RS_GraphicView::setCurrentAction: OK");
}


/**
 * Kills all running selection actions. Called when a selection action
 * is launched to reduce confusion.
 */
/*
void RS_GraphicView::killSelectActions() {
    if (eventHandler!=NULL) {
        eventHandler->killSelectActions();
    }
}
*/



/**
 * Kills all running actions. 
 */
void RS_GraphicView::killAllActions() {
    if (eventHandler!=NULL) {
        eventHandler->killAllActions();
    }
}



/**
 * Go back in menu or current action.
 */
void RS_GraphicView::back() {
    if (eventHandler!=NULL && eventHandler->hasAction()) {
        eventHandler->back();
    } else {
        if (RS_DIALOGFACTORY!=NULL) {
            RS_DIALOGFACTORY->requestPreviousMenu();
        }
    }
}



/**
 * Go forward with the current action.
 */
void RS_GraphicView::enter() {
    if (simulationRunning) {
        return;
    }
    
    if (eventHandler!=NULL && eventHandler->hasAction()) {
        eventHandler->enter();
    }
}



/**
 * Called by the actual GUI class which implements the RS_GraphicView 
 * interface to notify qcadlib about mouse events.
 */
void RS_GraphicView::mousePressEvent(RS_MouseEvent* e) {
    if (simulationRunning) {
        return;
    }

    if (eventHandler!=NULL) {
        eventHandler->mousePressEvent(e);
    }
}



/**
 * Called by the actual GUI class which implements the RS_GraphicView
 * interface to notify qcadlib about mouse events.
 */
void RS_GraphicView::mouseReleaseEvent(RS_MouseEvent* e) {
    if (simulationRunning) {
        return;
    }

    RS_DEBUG->print("RS_GraphicView::mouseReleaseEvent");
    if (eventHandler!=NULL) {
        if (RS2::qtToRsButtonState(e->button())!=RS2::RightButton || 
            eventHandler->hasAction()) {
            eventHandler->mouseReleaseEvent(e);
        }
        else {
            back();
            e->accept();
        }
    }
    RS_DEBUG->print("RS_GraphicView::mouseReleaseEvent: OK");
}



/**
 * Called by the actual GUI class which implements the RS_GraphicView
 * interface to notify qcadlib about mouse events.
 */
void RS_GraphicView::mouseMoveEvent(RS_MouseEvent* e) {
    if (simulationRunning) {
        return;
    }

    RS_Graphic* graphic = NULL;

    if (container!=NULL && container->rtti()==RS2::EntityGraphic) {
        graphic = dynamic_cast<RS_Graphic*>(container);
    }

    if (e!=NULL) {
        mx = e->x();
        my = e->y();
    }

    if (eventHandler!=NULL) {
        eventHandler->mouseMoveEvent(e);
    }

    if (eventHandler==NULL || !eventHandler->hasAction() && graphic!=NULL) {
        RS_Vector mouse = toGraph(RS_Vector(mx, my));
        RS_Vector relMouse = mouse - getRelativeZero();

        if (RS_DIALOGFACTORY!=NULL) {
            RS_DIALOGFACTORY->updateCoordinateWidget(mouse, relMouse);
        }
    }
}



/**
 * Called by the actual GUI class which implements the RS_GraphicView
 * interface to notify qcadlib about mouse events.
 */
void RS_GraphicView::mouseLeaveEvent() {
    if (simulationRunning) {
        return;
    }
    
    if (eventHandler!=NULL) {
        eventHandler->mouseLeaveEvent();
    }
}



/**
 * Called by the actual GUI class which implements the RS_GraphicView
 * interface to notify qcadlib about mouse events.
 */
void RS_GraphicView::mouseEnterEvent() {
    if (simulationRunning) {
        return;
    }
    
    if (eventHandler!=NULL) {
        eventHandler->mouseEnterEvent();
    }
}



/**
 * Called by the actual GUI class which implements the RS_GraphicView
 * interface to notify qcadlib about key events.
 */
void RS_GraphicView::keyPressEvent(RS_KeyEvent* e) {
    if (simulationRunning) {
        e->ignore();
        return;
    }
    
    if (eventHandler!=NULL) {
        eventHandler->keyPressEvent(e);
    }
    else {
        e->ignore();
    }
}



/**
 * Called by the actual GUI class which implements the RS_GraphicView
 * interface to notify qcadlib about key events.
 */
void RS_GraphicView::keyReleaseEvent(RS_KeyEvent* e) {
    if (simulationRunning) {
        e->ignore();
        return;
    }

    if (eventHandler!=NULL) {
        eventHandler->keyReleaseEvent(e);
    }
    else {
        e->ignore();
    }
}



/**
 * Called by the actual GUI class which implements a command line.
 */
void RS_GraphicView::commandEvent(RS_CommandEvent* e) {
    if (simulationRunning) {
        return;
    }
    
    if (eventHandler!=NULL) {
        eventHandler->commandEvent(e);
    }
}
    
    
    
void RS_GraphicView::dragEnterEvent(QDragEnterEvent* e) {
    if (simulationRunning) {
        return;
    }
    
    if (eventHandler!=NULL) {
        eventHandler->dragEnterEvent(e);
    }
}
    
    
    
void RS_GraphicView::dragMoveEvent(QDragMoveEvent* e) {
    if (simulationRunning) {
        return;
    }
    
    if (eventHandler!=NULL) {
        eventHandler->dragMoveEvent(e);
    }
}



void RS_GraphicView::dropEvent(QDropEvent* e) {
    if (simulationRunning) {
        return;
    }
    
    if (eventHandler!=NULL) {
        eventHandler->dropEvent(e);
    }
}



/**
 * Enables coordinate input in the command line.
 */
void RS_GraphicView::enableCoordinateInput() {
    if (eventHandler!=NULL) {
        eventHandler->enableCoordinateInput();
    }
}



/**
 * Disables coordinate input in the command line.
 */
void RS_GraphicView::disableCoordinateInput() {
    if (eventHandler!=NULL) {
        eventHandler->disableCoordinateInput();
    }
}



/**
 * zooms in by factor f
 */
void RS_GraphicView::zoomIn(double f, const RS_Vector& center) {

    if (!checkFactor(f)) {
        RS_DEBUG->print(RS_Debug::D_WARNING,
            "RS_GraphicView::zoomIn: invalid factor");
        return;
    }

    if (simulationRunning) {
        return;
    }

    RS_Vector c = center;
    if (c.valid==false) {
        c = toGraph(RS_Vector(getWidth()/2, getHeight()/2));
    }

    zoomWindow(
        toGraph(RS_Vector(0,0))
        .scale(c, RS_Vector(1.0/f,1.0/f)),
        toGraph(RS_Vector(getWidth(),getHeight()))
        .scale(c, RS_Vector(1.0/f,1.0/f)));
}



/**
 * zooms in by factor f in x
 */
void RS_GraphicView::zoomInX(double f) {
    if (simulationRunning) {
        return;
    }
    if (setFactorX(factor.x*f)) {
        offsetX=(int)((offsetX-getWidth()/2)*f)+getWidth()/2;
        adjustOffsetControls();
        adjustZoomControls();
        updateGrid();
        redraw();
    }
}



/**
 * zooms in by factor f in y
 */
void RS_GraphicView::zoomInY(double f) {
    if (simulationRunning) {
        return;
    }
    if (setFactorY(factor.y*f)) {
        offsetY=(int)((offsetY-getHeight()/2)*f)+getHeight()/2;
        adjustOffsetControls();
        adjustZoomControls();
        updateGrid();
        redraw();
    }
}



/**
 * zooms out by factor f
 */
void RS_GraphicView::zoomOut(double f, const RS_Vector& center) {
    if (simulationRunning) {
        return;
    }
    if (!checkFactor(f)) {
        RS_DEBUG->print(RS_Debug::D_WARNING,
            "RS_GraphicView::zoomOut: invalid factor");
        return;
    }
    zoomIn(1/f, center);
}



/**
 * zooms out by factor f in x
 */
void RS_GraphicView::zoomOutX(double f) {
    if (simulationRunning) {
        return;
    }
    if (!checkFactor(f)) {
        RS_DEBUG->print(RS_Debug::D_WARNING,
            "RS_GraphicView::zoomOutX: invalid factor");
        return;
    }
    if (setFactorX(factor.x/f)) {
        offsetX=(int)(offsetX/f);
        adjustOffsetControls();
        adjustZoomControls();
        updateGrid();
        redraw();
    }
}



/**
 * zooms out by factor f y
 */
void RS_GraphicView::zoomOutY(double f) {
    if (simulationRunning) {
        return;
    }
    if (!checkFactor(f)) {
        RS_DEBUG->print(RS_Debug::D_WARNING,
            "RS_GraphicView::zoomOutY: invalid factor");
        return;
    }
    if (setFactorY(factor.y/f)) {
        offsetY=(int)(offsetY/f);
        adjustOffsetControls();
        adjustZoomControls();
        updateGrid();
        redraw();
    }
}



/**
 * performs autozoom
 *
 * @param axis include axis in zoom
 * @param keepAspectRatio true: keep aspect ratio 1:1
 *                        false: factors in x and y are stretched to the max
 */
void RS_GraphicView::zoomAuto(bool axis, bool keepAspectRatio) {

    if (simulationRunning) {
        return;
    }
    RS_DEBUG->print("RS_GraphicView::zoomAuto");
    RS_DEBUG->print("RS_GraphicView::zoomAuto: borderTop: %d", borderTop);
    
    saveView();

    if (container!=NULL) {
        // 20080112: only auto zoom to visible entities
        container->calculateBorders(true);
        /*
        RS_Vector minV(RS_MAXDOUBLE,RS_MAXDOUBLE);
        RS_Vector maxV(RS_MINDOUBLE,RS_MINDOUBLE);

        for (RS_Entity* e=container->firstEntity();
                e!=NULL;
                e=container->nextEntity()) {

            if (e->isVisible()) {
                e->calculateBorders();
                if (!e->isContainer() || e->count()>0) {
                    minV = RS_Vector::minimum(e->getMin(),minV);
                    maxV = RS_Vector::maximum(e->getMax(),maxV);
                }
            }
        }
        */

        RS_Vector s;
        if (axis) {
            s.x = std::max(container->getMax().x, 0.0)
                 - std::min(container->getMin().x, 0.0);
            s.y = std::max(container->getMax().y, 0.0)
                 - std::min(container->getMin().y, 0.0);
        } else {
            s = container->getSize();
        }

        RS_Vector f;

        if (s.x>RS_TOLERANCE) {
            f.x = (getWidth()-borderLeft-borderRight) / s.x;
        } else {
            f.x = 1.0;
        }

        if (s.y>RS_TOLERANCE) {
            f.y = (getHeight()-borderTop-borderBottom) / s.y;
        } else {
            f.y = 2.0;
        }

        if (keepAspectRatio) {
            f.x = f.y = std::min(f.x, f.y);
        }

        if (f.x<RS_TOLERANCE) {
            f.x=f.y=1.0;
        }
        setFactorX(f.x);
        setFactorY(f.y);

        centerOffsetX();
        centerOffsetY();

        adjustOffsetControls();
        adjustZoomControls();
        updateGrid();
        redraw();
    }
    RS_DEBUG->print("RS_GraphicView::zoomAuto OK");
}




/**
 * Performs autozoom to selection.
 */
void RS_GraphicView::zoomSelection() {

    if (simulationRunning) {
        return;
    }

    RS_DEBUG->print("RS_GraphicView::zoomSelection");
    
    saveView();

    if (container!=NULL) {
        RS_Vector minV(RS_MAXDOUBLE,RS_MAXDOUBLE);
        RS_Vector maxV(RS_MINDOUBLE,RS_MINDOUBLE);

        for (RS_Entity* e=container->firstEntity();
                e!=NULL;
                e=container->nextEntity()) {

            if (e->isVisible() && e->isSelected()) {
                e->calculateBorders();
                if (!e->isContainer() || e->count()>0) {
                    minV = RS_Vector::minimum(e->getMin(),minV);
                    maxV = RS_Vector::maximum(e->getMax(),maxV);
                }
            }
        }

        RS_Vector s = maxV - minV;
        RS_Vector f;

        if (s.x>RS_TOLERANCE) {
            f.x = (getWidth()-borderLeft-borderRight) / s.x;
        } else {
            f.x = 1.0;
        }

        if (s.y>RS_TOLERANCE) {
            f.y = (getHeight()-borderTop-borderBottom) / s.y;
        } else {
            f.y = 1.0;
        }

        f.x = f.y = std::min(f.x, f.y);

        if (f.x<RS_TOLERANCE) {
            f.x=f.y=1.0;
        }

        setFactorX(f.x);
        setFactorY(f.y);

        adjustOffsetControls();
        adjustZoomControls();
        //centerOffsetX();
        //centerOffsetY();
        offsetX = (int)(((getWidth()-borderLeft-borderRight)
                         - (s.x*factor.x))/2.0
                        - (minV.x*factor.x)) + borderLeft;
        offsetY = (int)((getHeight()-borderTop-borderBottom
                         - (s.y*factor.y))/2.0
                        - (minV.y*factor.y)) + borderBottom;
        updateGrid();
        redraw();
    }

    RS_DEBUG->print("RS_GraphicView::zoomSelection OK");
}



/**
 * Shows previous view. 
 */
void RS_GraphicView::zoomPrevious() {

    RS_DEBUG->print("RS_GraphicView::zoomPrevious");
    if (simulationRunning) {
        return;
    }

    if (container!=NULL) {
        restoreView();
    }
}



/**
 * Saves the current view as previous view to which we can 
 * switch back later with @see restoreView(). 
 */
void RS_GraphicView::saveView() {
    previousOffsetX = offsetX;
    previousOffsetY = offsetY;
    previousFactor = factor;
}



/**
 * Restores the view previously saved with 
 * @see saveView(). 
 */
void RS_GraphicView::restoreView() {
    int pox = previousOffsetX;
    int poy = previousOffsetY;
    RS_Vector pf = previousFactor;

    saveView();
    
    offsetX = pox;
    offsetY = poy;
    factor = pf;
    
    adjustOffsetControls();
    adjustZoomControls();
    updateGrid();
    redraw();
}



/**
 * performs autozoom in y only
 *
 * @param axis include axis in zoom
 */
void RS_GraphicView::zoomAutoY(bool axis) {
    if (simulationRunning) {
        return;
    }
    if (container!=NULL) {
        double visibleHeight = 0.0;
        double minY = RS_MAXDOUBLE;
        double maxY = RS_MINDOUBLE;
        bool noChange = false;

        for (RS_Entity* e=container->firstEntity(RS2::ResolveNone);
                e!=NULL;
                e = container->nextEntity(RS2::ResolveNone)) {

            if (e->rtti()==RS2::EntityLine) {
                RS_Line* l = dynamic_cast<RS_Line*>(e);
                double x1, x2;
                x1 = toGuiX(l->getStartpoint().x);
                x2 = toGuiX(l->getEndpoint().x);
                if (x1>0.0 && x1<(double)getWidth() ||
                        x2>0.0 && x2<(double)getWidth()) {
                    minY = std::min(minY, l->getStartpoint().y);
                    minY = std::min(minY, l->getEndpoint().y);
                    maxY = std::max(maxY, l->getStartpoint().y);
                    maxY = std::max(maxY, l->getEndpoint().y);
                }
            }
        }

        if (axis) {
            visibleHeight = std::max(maxY, 0.0) - std::min(minY, 0.0);
        } else {
            visibleHeight = maxY-minY;
        }

        if (visibleHeight<1.0) {
            noChange = true;
        }

        double fy = 1.0;
        if (visibleHeight>1.0e-6) {
            fy = (getHeight()-borderTop-borderBottom)
                 / visibleHeight;
            if (factor.y<0.000001) {
                noChange = true;
            }
        }

        if (noChange==false) {
            if (setFactorY(fy)) {
                //centerOffsetY();
                offsetY = (int)((getHeight()-borderTop-borderBottom
                                 - (visibleHeight*factor.y))/2.0
                                - (minY*factor.y)) + borderBottom;
                adjustOffsetControls();
                adjustZoomControls();
                updateGrid();
            }
        }
        RS_DEBUG->print("Auto zoom y ok");
    }
}



/**
 * Zooms the area given by v1 and v2.
 *
 * @param keepAspectRatio true: keeps the aspect ratio 1:1
 *                        false: zooms exactly the selected range to the
 *                               current graphic view
 */
void RS_GraphicView::zoomWindow(RS_Vector v1, RS_Vector v2,
                                bool keepAspectRatio) {

    if (simulationRunning) {
        return;
    }

    saveView();
    
    double zoomX=480.0;    // Zoom for X-Axis
    double zoomY=640.0;    // Zoom for Y-Axis   (Set smaller one)
    double dum;            // Dummy for switching values
    int zoomBorder = 0;

    // Switch left/right and top/bottom is necessary:
    if(v1.x>v2.x) {
        dum=v1.x;
        v1.x=v2.x;
        v2.x=dum;
    }
    if(v1.y>v2.y) {
        dum=v1.y;
        v1.y=v2.y;
        v2.y=dum;
    }

    // Get zoom in X and zoom in Y:
    if(v2.x-v1.x>1.0e-6) {
        zoomX = getWidth() / (v2.x-v1.x);
    }
    if(v2.y-v1.y>1.0e-6) {
        zoomY = getHeight() / (v2.y-v1.y);
    }

    // Take smaller zoom:
    if (keepAspectRatio) {
        if(zoomX<zoomY) {
            if(getWidth()!=0) {
                zoomX = zoomY = ((double)(getWidth()-2*zoomBorder)) /
                                (double)getWidth()*zoomX;
            }
        } else {
            if(getHeight()!=0) {
                zoomX = zoomY = ((double)(getHeight()-2*zoomBorder)) /
                                (double)getHeight()*zoomY;
            }
        }
    }

    if(zoomX<0.0) {
        zoomX*=-1;
    }
    if(zoomY<0.0) {
        zoomY*=-1;
    }

    // Borders in pixel after zoom
    int pixLeft  =(int)(v1.x*zoomX);
    int pixTop   =(int)(v2.y*zoomY);
    int pixRight =(int)(v2.x*zoomX);
    int pixBottom=(int)(v1.y*zoomY);

    // Set new offset for zero point:
    if (setFactorX(zoomX) && setFactorY(zoomY)) {
        offsetX = - pixLeft + (getWidth() -pixRight +pixLeft)/2;
        offsetY = - pixTop + (getHeight() -pixBottom +pixTop)/2;
        adjustOffsetControls();
        adjustZoomControls();
        updateGrid();
        redraw();
    }
}



/**
 * Centers the point v1.
 */
void RS_GraphicView::zoomPan(int dx, int dy) {
    if (simulationRunning) {
        return;
    }

    offsetX += dx;
    offsetY -= dy;

    disableUpdate();
    adjustOffsetControls();
    updateGrid();
    enableUpdate();
    redraw();
}



/**
 * Scrolls in the given direction.
 */
void RS_GraphicView::zoomScroll(RS2::Direction direction) {
    if (simulationRunning) {
        return;
    }
    switch (direction) {
    case RS2::Up:
        offsetY-=50;
        break;
    case RS2::Down:
        offsetY+=50;
        break;
    case RS2::Right:
        offsetX+=50;
        break;
    case RS2::Left:
        offsetX-=50;
        break;
    }
    adjustOffsetControls();
    adjustZoomControls();
    updateGrid();
    redraw();
}



/**
 * Zooms to page extends.
 */
void RS_GraphicView::zoomPage() {

    RS_DEBUG->print("RS_GraphicView::zoomPage");
    if (container==NULL) {
        return;
    }
    if (simulationRunning) {
        return;
    }

    RS_Graphic* graphic = container->getGraphic();
    if (graphic==NULL) {
        return;
    }

    RS_Vector s = graphic->getPaperSize();
    RS_Vector pinsbase = graphic->getPaperInsertionBase();

    double fx, fy;

    if (s.x>RS_TOLERANCE) {
        fx = (getWidth()-borderLeft-borderRight) / s.x;
    } else {
        fx = 1.0;
    }

    if (s.y>RS_TOLERANCE) {
        fy = (getHeight()-borderTop-borderBottom) / s.y;
    } else {
        fy = 1.0;
    }

    fx = fy = std::min(fx, fy);

    if (fx<RS_TOLERANCE) {
        fx=fy=1.0;
    }

    setFactorX(fx);
    setFactorY(fy);

    centerOffsetX();
    centerOffsetY();
    adjustOffsetControls();
    adjustZoomControls();
    updateGrid();
    redraw();
}



/**
 * Draws the entities. If painter is NULL a new painter will 
 * be created and destroyed.
 */
void RS_GraphicView::drawIt() {
    if (!isUpdateEnabled()) {
        return;
    }
    if (simulationRunning) {
        return;
    }
    bool painterCreated = false;

    RS_SETTINGS->beginGroup("/Appearance");
    draftMode = (bool)RS_SETTINGS->readNumEntry("/DraftMode", 0);
    RS_SETTINGS->endGroup();

    RS_DEBUG->print("RS_GraphicView::drawIt");

    if (painter==NULL) {
#ifdef RS_DIRECTPAINTER
        createDirectPainter();
        painter->erase();
#else

        createPainter();
#endif

        painterCreated = true;
    } else {
        painter->erase();
    }

    // drawing paper border:
    if (isPrintPreview()) {
        drawPaper();
    }

    // drawing meta grid:
    if (!isPrintPreview()) {
        drawMetaGrid();
    }

    // drawing entities:
    drawEntity(container, 0.0, true);

    // drawing zero points:
    if (!isPrintPreview()) {
        drawAbsoluteZero();
    }

    // drawing grid:
    if (!isPrintPreview()) {
        drawGrid();
    }

    if (painterCreated==true) {
        destroyPainter();
    }

    RS_Graphic* graphic = getGraphic();
    if (graphic!=NULL) {
        RS_DEBUG->print("RS_GraphicView::drawIt: mod: %d",
            (int)graphic->isModifiedSinceRedraw());
    }
    
    RS_DEBUG->print("RS_GraphicView::drawIt: OK");
}


/**
 * Sets the pen of the painter object to the suitable pen for the given
 * entity.
 */
void RS_GraphicView::setPenForEntity(RS_Entity* e) {
    if (drawingMode==RS2::ModePreview) {
        return;
    }

    // set color of entity
    if (painter!=NULL && !painter->isPreviewMode()) {

        // Getting pen from entity (or layer)
        RS_Pen pen = e->getPen(true);

        // use preset fixed screen width:
        //if (pen.getWidth()==RS2::WidthScreen) {
        //}

        // scale pen width:
        //else 
        if (!draftMode && container!=NULL) {
            int w = pen.getWidth();
            if (w<0) {
                w = 0;
            }
            
            double uf = 1.0;  // unit factor
            double wf = 1.0;  // width factor
            RS_Graphic* graphic = container->getGraphic();
            if (graphic!=NULL) {
                uf = RS_Units::convert(1.0, RS2::Millimeter, graphic->getUnit());
                if ((isPrinting() || isPrintPreview()) &&
                        graphic->getPaperScale()>1.0e-6) {

                    wf = 1.0/graphic->getPaperScale();
                }
            }

            pen.setScreenWidth(toGuiDX(w/100.0*uf*wf));
        } else {
            pen.setScreenWidth(0);
        }


        // prevent drawing with 1-width which is slow:
        if (!isPrinting() && RS_Math::mround(pen.getScreenWidth())==1) {
            pen.setScreenWidth(0.0);
        }

#ifdef Q_OS_MACX
        // prevent printing with zero-width under mac os x:
        if (isPrinting() && RS_Math::mround(pen.getScreenWidth())==0) {
            pen.setScreenWidth(1);
        }
#endif

        // prevent background color on background drawing:
        if (pen.getColor().stripFlags()==background.stripFlags()) {
            pen.setColor(foreground);
        }

        // this entity is selected:
        if (e->isSelected() && e->isSelectionVisible()) {
            RS_DEBUG->print("RS_GraphicView::setPenForEntity: "
                "set selected pen");
            pen.setLineType(RS2::SolidLine);
            if (e->rtti()==RS2::EntityHatch) {
                pen.setColor(selectedColor.darker(133));
            }
            else {
                pen.setColor(selectedColor);
            }
        }

        // this entity is highlighted:
        if (e->isHighlighted()) {
            pen.setColor(highlightedColor);
        }

        // deleting not drawing:
        if (getDeleteMode()) {
            pen.setColor(background);
        }

        /*
        // experimental: non-active layers transparent
        if (container->isOfType(RS2::EntityGraphic)) {
            RS_Graphic* g = (RS_Graphic*)container;
            if (e->getLayer()!=g->getActiveLayer()) {
                pen.makeTransparent(128);
            }
        }
        */

        painter->setPen(pen);
        painter->setOpacity(pen.getColor().alphaF());
    }
}

    
    
/**
 * @return True if the given entity is visible in the current view port 
 * of this view.
 */
bool RS_GraphicView::isVisible(RS_Entity* e) {
    // update is disabled:
    if (!isUpdateEnabled()) {
        RS_DEBUG->print("RS_GraphicView::isVisible: updates disabled");
        return false;
    }

    // given entity is NULL:
    if (e==NULL) {
        RS_DEBUG->print("RS_GraphicView::isVisible: NULL entity");
        return false;
    }

    // entity is not visible:
    if (!e->isVisible()) {
        RS_DEBUG->print("RS_GraphicView::isVisible: not visible");
        return false;
    }

    // test if the entity is in the viewport
    if (!e->isContainer() && !printing &&
            (toGuiX(e->getMax().x)<-50 || 
             toGuiX(e->getMin().x)>getWidth()+50 ||
             toGuiY(e->getMin().y)<-50 || 
             toGuiY(e->getMax().y)>getHeight()+50)) {
        RS_DEBUG->print("RS_GraphicView::isVisible: out of viewport");
        return false;
    }
    
#ifdef Q_OS_WIN
    if (!e->isContainer() && !printing &&
        (toGuiX(e->getMax().x)>25000 || 
         toGuiX(e->getMin().x)<-25000 ||
         toGuiY(e->getMin().y)>25000 || 
         toGuiY(e->getMax().y)<-25000)) {
        RS_DEBUG->print("RS_GraphicView::isVisible: "
            "could trigger overflow (windows)");
        return false;
    }
#endif
    
    return true;
}


/**
 * Draws an entity. Might be recusively called e.g. for polylines.
 * If the class wide painter is NULL a new painter will be created 
 * and destroyed afterwards.
 *
 * @param patternOffset Offset of line pattern (used for connected 
 *        lines e.g. in splines).
 * @param db Double buffering on (recommended) / off
 */
void RS_GraphicView::drawEntity(RS_Entity* e, 
    double patternOffset, bool db) {

    if (e==NULL) {
        RS_DEBUG->print("RS_GraphicView::drawEntity: entity is NULL");
        return;
    }
        
    if (!isVisible(e)) {
        RS_DEBUG->print("RS_GraphicView::drawEntity: not visible");
        return;
    }

    // was the painter instance created by this call?
    bool painterCreated = false;
    
    // create a temporary painter device
    if (painter==NULL) {
        if (db) {
            createPainter();
        } else {
            createDirectPainter();
        }
        painterCreated = true;
    }
    
    // set pen (color):
    setPenForEntity(e);

    // draft:
    if (draftMode) {
        // large texts as rectangles:
        if (e->rtti()==RS2::EntityText) {
            if (toGuiDX(dynamic_cast<RS_Text*>(e)->getHeight())<4 || e->countDeep()>100) {
                painter->drawRect(toGui(e->getMin()), toGui(e->getMax()));
            } else {
                drawEntityPlain(e, patternOffset);
            }
        } 
        
        // all images as rectangles:
        else if (e->rtti()==RS2::EntityImage) {
            painter->drawRect(toGui(e->getMin()), toGui(e->getMax()));
        } 
        
        // hide hatches:
        else if (e->rtti()==RS2::EntityHatch) {
            // 20071206: QCad >2.1.3.2: draw outline of solid fills:
            drawEntityPlain(e, patternOffset);
        }

        else {
            drawEntityPlain(e, patternOffset);
        }
    } 
    
    // normal:
    else {
        RS_DEBUG->print("RS_GraphicView::drawEntity: draw plain");

        drawEntityPlain(e, patternOffset);
    }

    if (painterCreated==true) {
        destroyPainter();
    }
}

    
    
void RS_GraphicView::drawEntityHandles(RS_Entity* e) {

    if (!isVisible(e)) {
        return;
    }

    // draw reference points:
    if (e->isSelected() && e->isSelectionVisible()) {
        if (!e->isParentContainerSelected()) {
            RS_VectorList s = e->getRefPoints();

            for (int i=0; i<s.count(); ++i) {
                int sz = -1;
                RS_Color col = RS_Color(0,0,255);
                if (e->rtti()==RS2::EntityPolyline) {
                    if (i==0 || i==s.count()-1) {
                        if (i==0) {
                            sz = 6;
                            col = RS_Color(0,64,255);
                        }
                        else {
                            sz = 6;
                            col = RS_Color(0,0,128);
                        }
                    }
                }
                if (getDeleteMode()) {
                    painter->drawHandle(toGui(s.get(i)), background, sz);
                } else {
                    painter->drawHandle(toGui(s.get(i)), col, sz);
                }
            }
        }
    }
}


/**
 * Deletes an entity with the background color. 
 * Might be recusively called e.g. for polylines.
 */
void RS_GraphicView::deleteEntity(RS_Entity* e) {
    setDeleteMode(true);
    drawEntity(e);
    setDeleteMode(false);
}



/**
 * Draws an entity. 
 * The painter must be initialized and all the attributes (pen) must be set.
 */
void RS_GraphicView::drawEntityPlain(RS_Entity* e, double patternOffset) {
    if (e==NULL) {
        RS_DEBUG->print("RS_GraphicView::drawEntityPlain: entity is NULL");
        return;
    }
        
    if (!e->isContainer() && !printing &&
        (toGuiX(e->getMax().x)<-50 || 
         toGuiX(e->getMin().x)>getWidth()+50 ||
         toGuiY(e->getMin().y)<-50 || 
         toGuiY(e->getMax().y)>getHeight()+50)) {
        return;
    }
    
#ifdef Q_OS_WIN
    if (!e->isContainer() && !printing &&
        (toGuiX(e->getMax().x)>25000 || 
         toGuiX(e->getMin().x)<-25000 ||
         toGuiY(e->getMin().y)>25000 || 
         toGuiY(e->getMax().y)<-25000)) {
        return;
    }
#endif
    
    
    e->draw(painter, this, patternOffset);
}



/**
 * Simulates this drawing in slow motion.
 */
void RS_GraphicView::simulateIt() {
    RS_DEBUG->print("RS_GraphicView::simulateIt");

    if (simulationRunning) {
        return;
    }

    simulationRunning = true;
    simulationLast = RS_Vector(0.0,0.0);

    destroyPainter();

    createSimPainter();
    painter->erase();

    // drawing paper border:
    if (isPrintPreview()) {
        drawPaper();
    }

    // drawing meta grid:
    if (!isPrintPreview()) {
        drawMetaGrid();
    }

    // drawing grid:
    if (!isPrintPreview()) {
        drawGrid();
    }

    // drawing entities:
    RS_Pen pen(foreground, RS2::Width00, RS2::SolidLine);
    simulateEntity(container, pen);
    
    preview->clear();
    redraw();

    // drawing zero points:
    if (!isPrintPreview()) {
        drawAbsoluteZero();
    }
            
    destroyPainter();

    simulationRunning = false;
}



/**
 * Simulates the given entity.
 *
 * @param smooth If true, the entity will be drawn slowly (pixel by pixel).
 */
void RS_GraphicView::simulateEntity(RS_Entity* e, const RS_Pen& pen) {
    RS_DEBUG->print("RS_GraphicView::simulateEntity");
    
    if (e==NULL) {
        RS_DEBUG->print("RS_GraphicView::simulateEntity: entity is NULL");
        return;
    }

    if (e->isContainer()) {
        RS_DEBUG->print("RS_GraphicView::simulateEntity: simulate container");

        RS_EntityContainer* ec = dynamic_cast<RS_EntityContainer*>(e);
        for (RS_Entity* en=ec->firstEntity(RS2::ResolveNone);
                en!=NULL;
                en = ec->nextEntity(RS2::ResolveNone)) {

            if (en->isVisible() && en->isUndone()==false) {

                // draw rapid move:
                if (en->isAtomic() && simulationRapid) {
                    RS_DEBUG->print("RS_GraphicView::simulateEntity: draw rapid move");
                    RS_Vector sp = dynamic_cast<RS_AtomicEntity*>(en)->getStartpoint();
                    if (sp.distanceTo(simulationLast)>1.0e-4) {
                        createSimPainter();
                        RS_Pen rpen(RS_Color(0,0,255), RS2::Width00, RS2::SolidLine);
                        RS_Line rapidLine(NULL, RS_LineData(simulationLast, sp));
                        simulateEntity(&rapidLine, rpen);
                    }
                }

                if (en->isHighlighted()) {
                    RS_Pen hpen(highlightedColor, RS2::Width00, RS2::SolidLine);
                    simulateEntity(en, hpen);
                } else {
                    simulateEntity(en, pen);
                }

                if (en->isAtomic()) {
                    simulationLast = dynamic_cast<RS_AtomicEntity*>(en)->getEndpoint();
                }

                if (!simulationSmooth) {
                    simulationDelay(true);
                }
            }
        }
    } else {
        if (simulationSmooth) {
            RS_DEBUG->print("RS_GraphicView::simulateEntity: smooth simulation");
            switch (e->rtti()) {
            case RS2::EntityLine: {
                    RS_Line* line = dynamic_cast<RS_Line*>(e);
                    drawLineSmooth(toGui(line->getStartpoint()),
                                   toGui(line->getEndpoint()),
                                   pen);
                    //simulationSpeed);
                }
                break;

            case RS2::EntityArc: {
                    RS_Arc* arc = dynamic_cast<RS_Arc*>(e);
                    drawArcSmooth(toGui(arc->getCenter()),
                                  toGuiDX(arc->getRadius()),
                                  arc->getAngle1(), arc->getAngle2(),
                                  arc->isReversed(),
                                  pen);
                }
                break;

            case RS2::EntityCircle: {
                    RS_Circle* circle = dynamic_cast<RS_Circle*>(e);
                    drawArcSmooth(toGui(circle->getCenter()),
                                  toGuiDX(circle->getRadius()),
                                  0.0, 2.0*M_PI,
                                  false,
                                  pen);
                }
                break;

            default:
                break;
            }
        } else {
            RS_DEBUG->print("RS_GraphicView::simulateEntity: draw a single entity");
            createSimPainter();
            painter->setPen(pen);
            drawEntityPlain(e);
        }
    }
}


/**
 * Delay for slow motion simulation.
 *
 * @param step true: stepping mode (entity by entity simulation). adds a delay.
 */
void RS_GraphicView::simulationDelay(bool step) {
    //RS_DEBUG->print("RS_GraphicView::simulationDelay");

    int delay;
    static double simulationDelay = -1.0;

    if (simulationDelay<0.0) {
        RS_SETTINGS->beginGroup("/CAM");
        simulationDelay = (double)RS_SETTINGS->readNumEntry("/SimulationDelay", 0);
        RS_SETTINGS->endGroup();
    }

    delay = (100-simulationSpeed);
    int maxCalls = simulationSpeed/10;
    
    static int call=0;
    
    if (call>=maxCalls) {
        redraw();
        call = 0;
    }
    else {
        call++;
    }
        
    for (int i=0; i<=delay*simulationDelay; i++) {
        RS_APP->processEvents(QEventLoop::AllEvents, 10);
    }
}



/**
 * Prepares the tool preview during simulation.
 */
void RS_GraphicView::prepareSimulationPreview(const RS_Vector& pos) {
    preview->clear();
    RS_Circle* circle = new RS_Circle(preview, 
        RS_CircleData(pos, toGraphDX(5)));
    circle->setPen(RS_Pen(RS_Color(128,128,128),
                          RS2::Width00,
                          RS2::SolidLine));
    preview->addEntity(circle);
}


/**
 * Draws a line slowly from (x1, y1) to (x2, y2). This is used for simulation only.
 */
void RS_GraphicView::drawLineSmooth(const RS_Vector& p1, const RS_Vector& p2, const RS_Pen& pen) {
    double alpha = p1.angleTo(p2);
    double xStep, yStep;
    bool  xIsOne;

    if (RS_Math::cmpDouble(alpha, 0.0) || RS_Math::cmpDouble(alpha, 2*M_PI)) {
        xStep = 1.0;
        yStep = 0.0;
        xIsOne=true;
    } else if (RS_Math::cmpDouble(alpha, M_PI/2.0)) {
        xStep = 0.0;
        yStep =1.0;
        xIsOne=false;
    } else if (RS_Math::cmpDouble(alpha, M_PI)) {
        xStep =-1.0;
        yStep = 0.0;
        xIsOne=true;
    } else if (RS_Math::cmpDouble(alpha, M_PI/2.0*3.0)) {
        xStep = 0.0;
        yStep =-1.0;
        xIsOne=false;
    } else if (fabs(p2.x-p1.x)>fabs(p2.y-p1.y)) {
        if (p2.x>p1.x) {
            xStep=1.0;
        } else {
            xStep=-1.0;
        }
        yStep = tan(alpha)*xStep;
        xIsOne=true;
    } else {
        if (p2.y>p1.y) {
            yStep=1.0;
        } else {
            yStep=-1.0;
        }
        xStep = yStep/tan(alpha);
        xIsOne=false;
    }

    double lx = p1.x;
    double ly = p1.y;

    do {
        if (lx>=0.0 && lx<=(double)getWidth() && ly>=0.0 && ly<=(double)getHeight()) {
            createSimPainter();
            painter->setPen(pen);
            painter->drawGridPoint(RS_Vector(lx, ly));

            prepareSimulationPreview(toGraph((int)lx, (int)ly));

            simulationDelay();
        }

        lx+=xStep;
        ly+=yStep;

    } while( ( xIsOne && ((lx>=p1.x && lx<=p2.x) || (lx>=p2.x && lx<=p1.x))) ||
             (!xIsOne && ((ly>=p1.y && ly<=p2.y) || (ly>=p2.y && ly<=p1.y))));
}



void RS_GraphicView::drawArcSmooth(const RS_Vector& center,
                                   double radius, double a1, double a2, bool rev,
                                   const RS_Pen& pen) {
    if (radius<=1.4) {
        createSimPainter();
        painter->setPen(pen);
        painter->drawGridPoint(center);
    } else {
        int ix1 = RS_Math::mround(center.x + cos(a1)*radius);
        int iy1 = RS_Math::mround(center.y - sin(a1)*radius);
        int ix2 = RS_Math::mround(center.x + cos(a2)*radius);
        int iy2 = RS_Math::mround(center.y - sin(a2)*radius);
        int k2x=0;            // Next point on circle
        int k2y=0;            //
        int k1x=ix1;          // Prev point on circle
        int k1y=iy1;          //
        double aStep;          // Angle Step (rad)
        double a;              // Actual Angle (rad)
        double a2cp = a2;      // Copy of a2

        if (1.0/(radius*factor.x)<=1.0) {
            aStep=atan(1.0/(radius*factor.x));
        } else {
            aStep = 0.01;
        }

        if (aStep<0.01) {
            aStep = 0.01;
        }

        if (!rev) {

            // Arc Counterclockwise:
            //
            if (a1>a2cp-0.01) {
                a2cp+=2*M_PI;
            }

            for (a=a1+aStep; a<=a2cp; a+=aStep) {
                k2x = RS_Math::mround(center.x+cos(a)*radius);
                k2y = RS_Math::mround(center.y-sin(a)*radius);
                createSimPainter();
                painter->setPen(pen);
                if ((k2x>=0 && k2x<=painter->getWidth() &&
                        k2y>=0 && k2y<=painter->getHeight()) ||
                        (k1x>=0 && k1x<=painter->getWidth() &&
                         k1y>=0 && k1y<=painter->getHeight())) {
                    painter->drawLine(RS_Vector(k1x, k1y), RS_Vector(k2x, k2y));
                    
                    prepareSimulationPreview(toGraph(k2x, k2y));
                    
                    simulationDelay();
                }

                k1x=k2x;
                k1y=k2y;
            }
            createSimPainter();
            painter->setPen(pen);
            painter->drawLine(RS_Vector(k2x, k2y), RS_Vector(ix2, iy2));
        }
        else {

            // Arc Clockwise:
            //
            if (a1<a2cp+0.01) {
                a2cp-=2*M_PI;
            }
            for (a=a1-aStep; a>=a2cp; a-=aStep) {
                k2x = RS_Math::mround(center.x+cos(a)*radius);
                k2y = RS_Math::mround(center.y-sin(a)*radius);
                createSimPainter();
                painter->setPen(pen);
                if ((k2x>=0 && k2x<=painter->getWidth() &&
                        k2y>=0 && k2y<=painter->getHeight()) ||
                        (k1x>=0 && k1x<=painter->getWidth() &&
                         k1y>=0 && k1y<=painter->getHeight())) {
                    painter->drawLine(RS_Vector(k1x, k1y), RS_Vector(k2x, k2y));
                    
                    prepareSimulationPreview(toGraph(k2x, k2y));

                    simulationDelay();
                }
                k1x=k2x;
                k1y=k2y;
            }
            createSimPainter();
            painter->setPen(pen);
            painter->drawLine(RS_Vector(k2x, k2y), RS_Vector(ix2, iy2));
        }
    }
}



/**
 * @return Pointer to the static pattern struct that belongs to the
 * given pattern type or NULL.
 */
RS_LineTypePattern* RS_GraphicView::getPattern(RS2::LineType t) {
    switch (t) {
    case RS2::SolidLine:
        return &patternSolidLine;
        break;

    case RS2::DotLine:
        return &patternDotLine;
        break;
    case RS2::DotLine2:
        return &patternDotLine2;
        break;
    case RS2::DotLineX2:
        return &patternDotLineX2;
        break;

    case RS2::DashLine:
        return &patternDashLine;
        break;
    case RS2::DashLine2:
        return &patternDashLine2;
        break;
    case RS2::DashLineX2:
        return &patternDashLineX2;
        break;

    case RS2::DashDotLine:
        return &patternDashDotLine;
        break;
    case RS2::DashDotLine2:
        return &patternDashDotLine2;
        break;
    case RS2::DashDotLineX2:
        return &patternDashDotLineX2;
        break;

    case RS2::DivideLine:
        return &patternDivideLine;
        break;
    case RS2::DivideLine2:
        return &patternDivideLine2;
        break;
    case RS2::DivideLineX2:
        return &patternDivideLineX2;
        break;

    case RS2::CenterLine:
        return &patternCenterLine;
        break;
    case RS2::CenterLine2:
        return &patternCenterLine2;
        break;
    case RS2::CenterLineX2:
        return &patternCenterLineX2;
        break;

    case RS2::BorderLine:
        return &patternBorderLine;
        break;
    case RS2::BorderLine2:
        return &patternBorderLine2;
        break;
    case RS2::BorderLineX2:
        return &patternBorderLineX2;
        break;

    case RS2::LineByLayer:
        return &patternBlockLine;
        break;
    case RS2::LineByBlock:
        return &patternBlockLine;
        break;
    default:
        break;
    }
    return NULL;
}



/**
 * This virtual method can be overwritten to draw the absolute 
 * zero. It's called from within drawIt(). The default implemetation
 * draws a simple red round zero point.
 *
 * @see drawIt()
 */
void RS_GraphicView::drawAbsoluteZero() {
    RS_DEBUG->print("RS_GraphicView::drawAbsoluteZero");

    bool painterCreated = false;

    // create a temporary painter device
    if (painter==NULL) {
        createDirectPainter();
        painterCreated = true;
    }

    int zr = 20;

    RS_Pen p(RS_Color(255,0,0), RS2::Width00, RS2::SolidLine);
    p.setScreenWidth(0);
    painter->setPen(p);

    painter->drawLine(RS_Vector(toGuiX(0.0)-zr,
                                toGuiY(0.0)),
                      RS_Vector(toGuiX(0.0)+zr,
                                toGuiY(0.0)));

    painter->drawLine(RS_Vector(toGuiX(0.0),
                                toGuiY(0.0)-zr),
                      RS_Vector(toGuiX(0.0),
                                toGuiY(0.0)+zr));

    if (painterCreated) {
        destroyPainter();
    }
    RS_DEBUG->print("RS_GraphicView::drawAbsoluteZero: OK");
}



/**
 * This virtual method can be overwritten to draw the relative 
 * zero point. It's called from within drawIt(). The default implemetation
 * draws a simple red round zero point.
 *
 * @see drawIt()
 */
void RS_GraphicView::drawRelativeZero() {
    RS_DEBUG->print("RS_GraphicView::drawRelativeZero");
    
    bool painterCreated = false;

    if (relativeZero.valid==false) {
        RS_DEBUG->print("RS_GraphicView::drawRelativeZero: pos not valid");
        return;
    }

    // create a temporary painter device
    if (painter==NULL) {
        createDirectPainter();
        painterCreated = true;
    }

    RS_Pen p(RS_Color(255,0,0), RS2::Width00, RS2::SolidLine);
    p.setScreenWidth(0);
    painter->setPen(p);

    int zr=5;

    painter->drawLine(RS_Vector(toGuiX(relativeZero.x)-zr,
                                toGuiY(relativeZero.y)),
                      RS_Vector(toGuiX(relativeZero.x)+zr,
                                toGuiY(relativeZero.y)));

    painter->drawLine(RS_Vector(toGuiX(relativeZero.x),
                                toGuiY(relativeZero.y)-zr),
                      RS_Vector(toGuiX(relativeZero.x),
                                toGuiY(relativeZero.y)+zr));

    painter->drawCircle(toGui(relativeZero), 5);

    if (painterCreated) {
        destroyPainter();
    }
    
    RS_DEBUG->print("RS_GraphicView::drawRelativeZero: OK");
}



/**
 * Draws the paper border (for print previews).
 *
 * @see drawIt()
 */
void RS_GraphicView::drawPaper() {
    bool painterCreated = false;

    if (container==NULL) {
        return;
    }

    RS_Graphic* graphic = container->getGraphic();
    if (graphic==NULL) {
        return;
    }

    if (graphic->getPaperScale()<1.0e-6) {
        return;
    }

    // create a temporary painter device
    if (painter==NULL) {
        createDirectPainter();
        painterCreated = true;
    }

    // draw paper:
    painter->setPen(RS_Pen(Qt::gray));

    RS_Vector pinsbase = graphic->getPaperInsertionBase();
    RS_Vector size = graphic->getPaperSize();
    double scale = graphic->getPaperScale();

    RS_Vector v1 = toGui((RS_Vector(0,0)-pinsbase)/scale);
    RS_Vector v2 = toGui((size-pinsbase)/scale);

    // gray background:
    painter->fillRect(0,0, getWidth(), getHeight(),
                      RS_Color(200,200,200));

    // shadow
    painter->fillRect(
        (int)(v1.x)+6, (int)(v1.y)+6,
        (int)((v2.x-v1.x)), (int)((v2.y-v1.y)),
        RS_Color(64,64,64));

    // border:
    painter->fillRect(
        (int)(v1.x), (int)(v1.y),
        (int)((v2.x-v1.x)), (int)((v2.y-v1.y)),
        RS_Color(64,64,64));

    // paper
    painter->fillRect(
        (int)(v1.x)+1, (int)(v1.y)-1,
        (int)((v2.x-v1.x))-2, (int)((v2.y-v1.y))+2,
        RS_Color(255,255,255));


    if (painterCreated) {
        destroyPainter();
    }
}


/**
 * Draws the grid.
 *
 * @see drawIt()
 */
void RS_GraphicView::drawGrid() {

    if (grid==NULL || isGridOn()==false) {
        return;
    }

    bool painterCreated = false;

    // create a temporary painter device
    if (painter==NULL) {
        createDirectPainter();
        painterCreated = true;
    }

    // draw grid:
    painter->setPen(gridColor);

    RS_Vector* pts = grid->getPoints();
    if (pts!=NULL) {
        for (int i=0; i<grid->count(); ++i) {
            painter->drawGridPoint(toGui(pts[i]));
        }
    }

    // draw grid info:
    RS_String info = grid->getInfo();

    updateGridStatusWidget(info);

    if (painterCreated) {
        destroyPainter();
    }
}



/**
 * Draws the meta grid.
 *
 * @see drawIt()
 */
void RS_GraphicView::drawMetaGrid() {

    if (grid==NULL || isGridOn()==false) {
        return;
    }

    bool painterCreated = false;

    // create a temporary painter device
    if (painter==NULL) {
        createDirectPainter();
        painterCreated = true;
    }
    
    RS_SETTINGS->beginGroup("/Appearance");
        bool solidGridLines = (bool)RS_SETTINGS->readNumEntry("/SolidGridLines", 
#ifdef Q_OS_MACX
    1
#else
    0
#endif
    );
    RS_SETTINGS->endGroup();

    RS_Pen pen;
    if (solidGridLines) {
        pen = RS_Pen(metaGridColor,
                     RS2::Width00,
                     RS2::SolidLine);
    }
    else {
        pen = RS_Pen(metaGridColor,
                     RS2::Width00,
                     RS2::DotLine);
    }
    painter->setPen(pen);

    // draw meta grid:
    double* mx = grid->getMetaX();
    if (mx!=NULL) {
        for (int i=0; i<grid->countMetaX(); ++i) {
            painter->drawLine(RS_Vector(toGuiX(mx[i]), 0),
                              RS_Vector(toGuiX(mx[i]), getHeight()));
        }
    }
    double* my = grid->getMetaY();
    if (my!=NULL) {
        for (int i=0; i<grid->countMetaY(); ++i) {
            painter->drawLine(RS_Vector(0, toGuiY(my[i])),
                              RS_Vector(getWidth(), toGuiY(my[i])));
        }
    }

    if (painterCreated) {
        destroyPainter();
    }
}


/**
 * Updates the grid if there is one. 
 */
void RS_GraphicView::updateGrid() {
    if (grid!=NULL) {
        grid->update();
    }
}



/**
 * Sets the default snap mode used by newly created actions.
 */
void RS_GraphicView::setDefaultSnapMode(RS2::ActionType sm) {
    defaultSnapMode = sm;
    if (eventHandler!=NULL) {
        eventHandler->setSnapMode(sm);
    }
}



/**
 * Sets a snap restriction (e.g. orthogonal).
 */
void RS_GraphicView::setSnapRestriction(RS2::ActionType sr) {
    defaultSnapRes = sr;

    if (eventHandler!=NULL) {
        eventHandler->setSnapRestriction(sr);
    }
}






/**
 * Translates a real coordinate in X to a screen coordinate X.
 * @param visible Pointer to a boolean which will contain true
 * after the call if the coordinate is within the visible range.
 */
double RS_GraphicView::toGuiX(double x, bool* visible) {
    if (visible!=NULL) {
        double res = x*factor.x+offsetX;
        if (res>0.0 && res<getWidth()) {
            *visible = true;
        } else {
            *visible = false;
        }
    }
    return x*factor.x + offsetX;
}



/**
 * Translates a real coordinate in Y to a screen coordinate Y.
 */
double RS_GraphicView::toGuiY(double y) {
    return -y*factor.y + getHeight() - offsetY;
}



/**
 * Translates a real coordinate distance to a screen coordinate distance.
 */
double RS_GraphicView::toGuiDX(double d) {
    return d*factor.x;
}



/**
 * Translates a real coordinate distance to a screen coordinate distance.
 */
double RS_GraphicView::toGuiDY(double d) {
    return d*factor.y;
}



/**
 * Translates a vector in screen coordinates to a vector in real coordinates.
 */
RS_Vector RS_GraphicView::toGraph(const RS_Vector& v) {
    return RS_Vector(toGraphX(RS_Math::mround(v.x)),
                     toGraphY(RS_Math::mround(v.y)), 0.0);
}



/**
 * Translates two screen coordinates to a vector in real coordinates.
 */
RS_Vector RS_GraphicView::toGraph(int x, int y) {
    return RS_Vector(toGraphX(x), toGraphY(y), 0.0);
}


/**
 * Translates a screen coordinate in X to a real coordinate X.
 */
double RS_GraphicView::toGraphX(int x) {
    return (x - offsetX)/factor.x;
}



/**
 * Translates a screen coordinate in Y to a real coordinate Y.
 */
double RS_GraphicView::toGraphY(int y) {
    return -(y - getHeight() + offsetY)/factor.y;
}



/**
 * Translates a screen coordinate distance to a real coordinate distance.
 */
double RS_GraphicView::toGraphDX(int d) {
    return d/factor.x;
}



/**
 * Translates a screen coordinate distance to a real coordinate distance.
 */
double RS_GraphicView::toGraphDY(int d) {
    return d/factor.y;
}



/**
 * Sets the relative zero coordinate (if not locked)
 * without deleting / drawing the point.
 */
void RS_GraphicView::setRelativeZero(const RS_Vector& pos) {
    if (relativeZeroLocked==false) {
        relativeZero = pos;
    }
}



/**
 * Sets the relative zero coordinate, deletes the old position
 * on the screen and draws the new one.
 */
void RS_GraphicView::moveRelativeZero(const RS_Vector& pos) {
    setRelativeZero(pos);
}


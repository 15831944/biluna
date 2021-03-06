/****************************************************************************
** $Id: rs_entitycontainer.cpp 9471 2008-03-28 11:12:04Z andrew $
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


#include "rs_entitycontainer.h"

//#include <values.h>

#include "rs_debug.h"
#include "rs_dimension.h"
#include "rs_math.h"
#include "rs_layer.h"
#include "rs_line.h"
#include "rs_leader.h"
#include "rs_modification.h"
#include "rs_polyline.h"
#include "rs_text.h"
#include "rs_insert.h"
#include "rs_spline.h"
#include "rs_information.h"
#include "rs_graphicview.h"

bool RS_EntityContainer::autoUpdateBorders = true;

/**
 * Default constructor.
 *
 * @param owner True if we own and also delete the entities.
 */
RS_EntityContainer::RS_EntityContainer(RS_EntityContainer* parentContainer,
                                       bool owner)
        : RS_Entity(parentContainer) {

    RS_DEBUG->print("RS_EntityContainer::RS_EntityContainer");
    
    entities.setAutoDelete(owner);
    subContainer = NULL;
    //autoUpdateBorders = true;

    RS_DEBUG->print("RS_EntityContainer::RS_EntityContainer: OK");
}



/**
 * Destructor.
 */
RS_EntityContainer::~RS_EntityContainer() {
    RS_DEBUG->print("RS_EntityContainer::~RS_EntityContainer");
    clear();
    RS_DEBUG->print("RS_EntityContainer::~RS_EntityContainer: OK");
}



RS_Entity* RS_EntityContainer::clone() {
    RS_EntityContainer* ec = 
        new RS_EntityContainer(parentContainer, entities.autoDelete());
    ec->copyPropertiesFrom(this);
    ec->entities = entities;
    ec->detach();
    ec->initId();
    return ec;
}



/**
 * Detaches shallow copies and creates deep copies of all subentities.
 * This function is called after cloning entity containers.
 */
void RS_EntityContainer::detach() {

    // make deep copies of all entities:
    RS_PtrList<RS_Entity> tmp;
    for (RS_Entity* e=firstEntity();
            e!=NULL;
            e=nextEntity()) {
        if (!e->getFlag(RS2::FlagTemp) && !e->isUndone()) {
            tmp.append(e->clone());
        }
    }

    // clear shared pointers:
    bool autoDel = entities.autoDelete();
    entities.setAutoDelete(false);
    entities.clear();
    entities.setAutoDelete(autoDel);

    // point to new deep copies:
    for (int i=0; i<tmp.size(); ++i) {
        RS_Entity* e = tmp[i];
        e->reparent(this);
        entities.append(e);
    }
}



void RS_EntityContainer::reparent(RS_EntityContainer* parentContainer) {
    RS_Entity::reparent(parentContainer);
}



/**
 * Forces the given pen on all subentities.
 */
void RS_EntityContainer::setOverridePen(const RS_Pen& pen) {
    RS_Entity::setOverridePen(pen);

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->setOverridePen(pen);
    }
}



/**
 * Set the layer of this entity all sub entities to the active layer.
 * Used for pasting to current layer.
 */
void RS_EntityContainer::setLayerToActive() {
    RS_Entity::setLayerToActive();

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->setLayerToActive();
    }
}



/**
 * Called when the undo state changed. Forwards the event to all sub-entities.
 *
 * @param undone true: entity has become invisible.
 *               false: entity has become visible.
 */
void RS_EntityContainer::undoStateChanged(bool undone) {

    RS_Entity::undoStateChanged(undone);

    // ! don't pass on to subentities. undo list handles them
    // All sub-entities:
    /*for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->setUndoState(undone);
}*/
}



void RS_EntityContainer::setVisible(bool v) {
    RS_Entity::setVisible(v);

    // All sub-entities:
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->setVisible(v);
    }
}



/**
 * @return Total length of all entities in this container.
 */
double RS_EntityContainer::getLength() {
    double ret = 0.0;

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        if (e->isVisible()) {
            double l = e->getLength();
            if (l<0.0) {
                ret = -1.0;
                break;
            } else {
                ret += l;
            }
        }
    }

    return ret;
}


/**
 * Resets all processed flags of all entities.
 */
void RS_EntityContainer::setProcessed(bool on) {
    RS_Entity::setProcessed(on);

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->setProcessed(on);
    }
}


/**
 * Selects this entity.
 */
bool RS_EntityContainer::setSelected(bool select, bool /*updateDependent*/) {
    // Try to select this entity:
    if (RS_Entity::setSelected(select)) {

        // Select all sub-entities:
        for (RS_Entity* e=firstEntity(RS2::ResolveNone);
                e!=NULL;
                e=nextEntity(RS2::ResolveNone)) {
            if (e->isVisible() /*&& e->rtti()!=RS2::EntityHatch*/) {
                e->setSelected(select);
            }
        }
        return true;
    } else {
        return false;
    }
}



/**
 * Sets the selection visibility.
 */
void RS_EntityContainer::setSelectionVisible(bool on) {
    RS_Entity::setSelectionVisible(on);

    // All sub-entity's:
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        if (e->isVisible() /*&& e->rtti()!=RS2::EntityHatch*/) {
            e->setSelectionVisible(on);
        }
    }
}



/**
 * Toggles select on this entity.
 */
bool RS_EntityContainer::toggleSelected() {
    // Toggle this entity's select:
    if (RS_Entity::toggleSelected()) {

        // Toggle all sub-entity's select:
        /*for (RS_Entity* e=firstEntity(RS2::ResolveNone);
                e!=NULL;
                e=nextEntity(RS2::ResolveNone)) {
            e->toggleSelected();
    }*/
        return true;
    } else {
        return false;
    }
}



/**
 * Selects all entities within the given area.
 *
 * @param select True to select, False to deselect the entities.
 */
void RS_EntityContainer::selectWindow(const RS_Vector& v1, const RS_Vector& v2,
                                      bool select) {

    bool included;
    bool cross = (v2.x<v1.x);

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {

        included = false;

        if (e->isVisible()) {
            if (e->isInWindow(v1, v2)) {
                //e->setSelected(select);
                included = true;
            } else if (cross==true) {
                RS_Line** l = new RS_Line*[4];
                l[0] = new RS_Line(NULL, RS_LineData(v1, RS_Vector(v2.x, v1.y)));
                l[1] = new RS_Line(NULL, RS_LineData(RS_Vector(v2.x, v1.y), v2));
                l[2] = new RS_Line(NULL, RS_LineData(v2, RS_Vector(v1.x, v2.y)));
                l[3] = new RS_Line(NULL, RS_LineData(RS_Vector(v1.x, v2.y), v1));
                RS_VectorList sol;

                if (e->isContainer()) {
                    RS_EntityContainer* ec = dynamic_cast<RS_EntityContainer*>(e);
                    for (RS_Entity* se=ec->firstEntity(RS2::ResolveAll);
                            se!=NULL && included==false;
                            se=ec->nextEntity(RS2::ResolveAll)) {

                        for (int i=0; i<4; ++i) {
                            sol = RS_Information::getIntersection(
                                      se, l[i], true);
                            if (sol.hasValid()) {
                                included = true;
                                break;
                            }
                        }
                    }
                } else {
                    for (int i=0; i<4; ++i) {
                        sol = RS_Information::getIntersection(e, l[i], true);
                        if (sol.hasValid()) {
                            included = true;
                            break;
                        }
                    }
                }

                for (int i=0; i<4; ++i) {
                    delete l[i];
                }
                delete[] l;
            }
        }

        if (included) {
            e->setSelected(select);
        }
    }
}



/**
 * Adds a entity to this container and updates the borders of this 
 * entity-container if autoUpdateBorders is true.
 */
void RS_EntityContainer::addEntity(RS_Entity* entity) {
    if (entity==NULL) {
        return;
    }

    if (isSelected()) {
        entity->setSelected(true);
    }

    if (entity->alwaysPrepend()) {
        entities.prepend(entity);
    }
    else {
        entities.append(entity);
    }
    
    entity->addedEvent(this);
    
    if (autoUpdateBorders) {
        adjustBorders(entity);
    }
}


/**
 * Inserts a entity to this container at the given position and updates 
 * the borders of this entity-container if autoUpdateBorders is true.
 */
void RS_EntityContainer::insertEntity(int index, RS_Entity* entity) {
    if (entity==NULL) {
        return;
    }

    entities.insert(index, entity);

    if (autoUpdateBorders) {
        adjustBorders(entity);
    }
}



/**
 * Replaces the entity at the given index with the given entity
 * and updates the borders of this entity-container if autoUpdateBorders is true.
 */
void RS_EntityContainer::replaceEntity(int index, RS_Entity* entity) {
    if (entity==NULL) {
        RS_DEBUG->print("RS_EntityContainer::replaceEntity: entity is NULL");
        return;
    }

    entities.replace(index, entity);

    if (autoUpdateBorders) {
        adjustBorders(entity);
    }
}



/**
 * Removes an entity from this container and updates the borders of 
 * this entity-container if autoUpdateBorders is true.
 */
bool RS_EntityContainer::removeEntity(RS_Entity* entity) {
    int idx = entities.indexOf(entity);
    if (idx==-1) {
        return false;
    }
    else {
        /*
        if (getDocument()!=NULL) {
            //getDocument()->removeUndoablePointers(entity);
        }
        */
        if (isOfType(RS2::EntityDocument)) {
            dynamic_cast<RS_Document*>(this)->removeUndoablePointers(entity);
        }
        entities.removeAt(idx);
    }
    if (autoUpdateBorders) {
        calculateBorders();
    }
    return true;
}



/**
 * Moves the given entity in the background (at the beginning of the entity list).
 */    
void RS_EntityContainer::moveEntityToBack(RS_Entity* entity, bool toBack) {
    int idx = entities.indexOf(entity);
    if (idx!=-1) {
        if (toBack) {
            entities.prepend(entities.takeAt(idx));
        }
        else {
            entities.append(entities.takeAt(idx));
        }
    }
}



/**
 * Moves the given entity in the foreground (at the end of the entity list).
 */    
void RS_EntityContainer::moveEntityToFront(RS_Entity* entity) {
    moveEntityToBack(entity, false);
}


/**
 * Erases all entities in this container and resets the borders..
 */
void RS_EntityContainer::clear() {
    entities.clear();
    resetBorders();
}


/**
 * Counts all entities (branches of the tree). 
 */
int RS_EntityContainer::count() {
    return entities.count();
}


/**
 * Counts all entities (leaves of the tree). 
 */
int RS_EntityContainer::countDeep() {
    int c=0;

    for (RS_Entity* t=firstEntity(RS2::ResolveNone);
            t!=NULL;
            t=nextEntity(RS2::ResolveNone)) {
        c+=t->countDeep();
    }

    return c;
}



/**
 * Counts the selected entities in this container.
 */
int RS_EntityContainer::countSelected() {
    int c=0;

    for (RS_Entity* t=firstEntity(RS2::ResolveNone);
            t!=NULL;
            t=nextEntity(RS2::ResolveNone)) {

        if (t->isSelected()) {
            c++;
        }
    }

    return c;
}



/**
 * Adjusts the borders of this graphic (max/min values)
 */
void RS_EntityContainer::adjustBorders(RS_Entity* entity) {
    if (entity!=NULL && !entity->isUndone()) {
        // make sure a container is not empty (otherwise the border
        //   would get extended to 0/0):
        if (!entity->isContainer() || entity->count()>0) {
            minV = RS_Vector::minimum(entity->getMin(),minV);
            maxV = RS_Vector::maximum(entity->getMax(),maxV);
        }
    }
}


/**
 * Recalculates the borders of this entity container.
 */
void RS_EntityContainer::calculateBorders(bool visibleOnly) {
    // optimization 20071205:
    if (!isUpdateEnabled()) {
        return;
    }

    resetBorders();
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {

        if (!e->isUndone()) {
            if (!visibleOnly || e->isVisible()) {
                e->calculateBorders();
                adjustBorders(e);
            }
        }
    }

    // needed for correcting corrupt data (PLANS.dxf)
    if (minV.x>maxV.x || minV.x>RS_MAXDOUBLE || maxV.x>RS_MAXDOUBLE
            || minV.x<RS_MINDOUBLE || maxV.x<RS_MINDOUBLE) {

        minV.x = 0.0;
        maxV.x = 0.0;
    }
    if (minV.y>maxV.y || minV.y>RS_MAXDOUBLE || maxV.y>RS_MAXDOUBLE
            || minV.y<RS_MINDOUBLE || maxV.y<RS_MINDOUBLE) {

        minV.y = 0.0;
        maxV.y = 0.0;
    }
        
    RS_DEBUG->print("RS_EntityContainer::calculateBorders: "
        "%f/%f - %f/%f", minV.x, minV.y, maxV.x, maxV.y);
}



/**
 * Updates all Dimension entities in this container and 
 * reposition their labels if autoText is true.
 * If onlySelected is true, only selected dimensions will
 * be modified, or all if nothing is selected.
 *
 * @return Number of entities that were updated.
 */
int RS_EntityContainer::updateDimensions(bool autoText, bool onlySelected) {

    RS_DEBUG->print("RS_EntityContainer::updateDimensions()");

    if (onlySelected && countSelected()==0) {
        onlySelected = false;
    }

    int num = 0;
    RS_PtrList<RS_Entity>::iterator e;
    for (e = entities.begin(); e != entities.end(); ++e) {
        if (!(*e)->isUndone()) {
            if (!onlySelected || (*e)->isSelected()) {
                if ((*e)->isOfType(RS2::EntityDimension)) {
                    (*e)->setSelected(false);
                    // update and reposition label:
                    dynamic_cast<RS_Dimension*>(*e)->update(autoText);
                    num++;
                } else if ((*e)->rtti()==RS2::EntityDimLeader) {
                    (*e)->setSelected(false);
                    dynamic_cast<RS_Leader*>(*e)->update();
                    num++;
                } else if ((*e)->isContainer()) {
                    num += dynamic_cast<RS_EntityContainer*>(*e)->
                        updateDimensions(autoText, onlySelected);
                }
            }
        }
    }

    RS_DEBUG->print("RS_EntityContainer::updateDimensions() OK");

    return num;
}



/**
 * Updates all Insert entities in this container.
 */
void RS_EntityContainer::updateInserts() {

    RS_DEBUG->print("RS_EntityContainer::updateInserts()");

    //for (RS_Entity* e=firstEntity(RS2::ResolveNone);
    //        e!=NULL;
    //        e=nextEntity(RS2::ResolveNone)) {
    
    RS_PtrList<RS_Entity>::iterator e;
    for (e = entities.begin(); e != entities.end(); ++e) {
        //// Only update our own inserts and not inserts of inserts
        if ((*e)->rtti()==RS2::EntityInsert) {
            dynamic_cast<RS_Insert*>(*e)->update();
        } else if ((*e)->isContainer() && 
            (*e)->rtti()!=RS2::EntityHatch) {
            dynamic_cast<RS_EntityContainer*>(*e)->updateInserts();
        }
    }

    RS_DEBUG->print("RS_EntityContainer::updateInserts() OK");
}



/**
 * Renames all inserts with name 'oldName' to 'newName'. This is
 *   called after a block was rename to update the inserts.
 */
void RS_EntityContainer::renameInserts(const RS_String& oldName,
                                       const RS_String& newName) {
    RS_DEBUG->print("RS_EntityContainer::renameInserts()");

    RS_PtrList<RS_Entity>::iterator e;
    for (e = entities.begin(); e != entities.end(); ++e) {
        if ((*e)->rtti()==RS2::EntityInsert) {
            RS_Insert* i = dynamic_cast<RS_Insert*>(*e);
            if (i->getName()==oldName) {
                i->setName(newName);
            }
        } else if ((*e)->isContainer()) {
            dynamic_cast<RS_EntityContainer*>(*e)->renameInserts(oldName, newName);
        }
    }

    RS_DEBUG->print("RS_EntityContainer::renameInserts() OK");

}



/**
 * Updates all Spline entities in this container.
 */
void RS_EntityContainer::updateSplines() {

    RS_DEBUG->print("RS_EntityContainer::updateSplines()");

    RS_PtrList<RS_Entity>::iterator e;
    for (e = entities.begin(); e != entities.end(); ++e) {
        if ((*e)->rtti()==RS2::EntitySpline) {
            dynamic_cast<RS_Spline*>(*e)->update();
        } else if ((*e)->isContainer() && (*e)->rtti()!=RS2::EntityHatch) {
            dynamic_cast<RS_EntityContainer*>(*e)->updateSplines();
        }
    }

    RS_DEBUG->print("RS_EntityContainer::updateSplines() OK");
}


/**
 * Updates the sub entities of this container. 
 */
void RS_EntityContainer::update() {
    RS_DEBUG->print("RS_EntityContainer::update");
    RS_PtrList<RS_Entity>::iterator e;
    for (e = entities.begin(); e != entities.end(); ++e) {
        (*e)->update();
    }
}



/**
 * Returns the first entity or NULL if this graphic is empty.
 * @param level 
 */
RS_Entity* RS_EntityContainer::firstEntity(RS2::ResolveLevel level) {
    switch (level) {
    case RS2::ResolveNone:
        return entities.first();
        break;

    case RS2::ResolveAll: {
            subContainer=NULL;
            RS_Entity* e = entities.first();
            if (e!=NULL && e->isContainer()) {
                subContainer = dynamic_cast<RS_EntityContainer*>(e);
                e = subContainer->firstEntity(level);
                // emtpy container:
                if (e==NULL) {
                    subContainer = NULL;
                    e = nextEntity(level);
                }
            }
            return e;
        }
        break;
    }

    return NULL;
}



/**
 * Returns the last entity or \p NULL if this graphic is empty.
 *
 * @param level \li \p 0 Groups are not resolved
 *              \li \p 1 (default) only Groups are resolved
 *              \li \p 2 all Entity Containers are resolved
 */
RS_Entity* RS_EntityContainer::lastEntity(RS2::ResolveLevel level) {
    switch (level) {
    case RS2::ResolveNone:
        return entities.last();
        break;

    case RS2::ResolveAll: {
            RS_Entity* e = entities.last();
            subContainer = NULL;
            if (e!=NULL && e->isContainer()) {
                subContainer = dynamic_cast<RS_EntityContainer*>(e);
                e = dynamic_cast<RS_EntityContainer*>(e)->lastEntity(level);
            }
            return e;
        }
        break;
    }

    return NULL;
}



/**
 * Returns the next entity or container or \p NULL if the last entity 
 * returned by \p next() was the last entity in the container.
 */
RS_Entity* RS_EntityContainer::nextEntity(RS2::ResolveLevel level) {
    switch (level) {
    case RS2::ResolveNone:
        return entities.next();
        break;

    case RS2::ResolveAll: {
            RS_Entity* e=NULL;
            if (subContainer!=NULL) {
                e = subContainer->nextEntity(level);
                if (e!=NULL) {
                    return e;
                } else {
                    e = entities.next();
                }
            } else {
                e = entities.next();
            }
            if (e!=NULL && e->isContainer()) {
                subContainer = dynamic_cast<RS_EntityContainer*>(e);
                e = dynamic_cast<RS_EntityContainer*>(e)->firstEntity(level);
                // emtpy container:
                if (e==NULL) {
                    subContainer = NULL;
                    e = nextEntity(level);
                }
            }
            return e;
        }
        break;
    }
    return NULL;
}



/**
 * Returns the prev entity or container or \p NULL if the last entity 
 * returned by \p prev() was the first entity in the container.
 */
RS_Entity* RS_EntityContainer::prevEntity(RS2::ResolveLevel level) {
    switch (level) {

    case RS2::ResolveNone:
        return entities.prev();
        break;
    
    case RS2::ResolveAll: {
            RS_Entity* e=NULL;
            if (subContainer!=NULL) {
                e = subContainer->prevEntity(level);
                if (e!=NULL) {
                    return e;
                } else {
                    e = entities.prev();
                }
            } else {
                e = entities.prev();
            }
            if (e!=NULL && e->isContainer()) {
                subContainer = dynamic_cast<RS_EntityContainer*>(e);
                e = dynamic_cast<RS_EntityContainer*>(e)->lastEntity(level);
                // emtpy container:
                if (e==NULL) {
                    subContainer = NULL;
                    e = prevEntity(level);
                }
            }
            return e;
        }
    }
    return NULL;
}



/**
 * @return Entity at the given index or NULL if the index is out of range.
 */
RS_Entity* RS_EntityContainer::entityAt(int index) {
    entities.setCurrentIndex(index);
    return entities[index];
}



/**
 * @return Current index.
 */
int RS_EntityContainer::entityAt() {
    return entities.currentIndex();
}


/**
 * Finds the given entity and makes it the current entity if found.
 */
int RS_EntityContainer::findEntity(RS_Entity* entity) {
    if (entity!=NULL) {
        entities.setCurrent(entity);
        return entities.currentIndex();
    }
    else {
        return -1;
    }
}



/**
 * @return The current entity.
 */
RS_Entity* RS_EntityContainer::currentEntity() {
    return entities.current();
}


/**
 * Returns the copy to a new iterator for traversing the entities.
 */
RS_MutableListIterator<RS_Entity*> RS_EntityContainer::createIterator() {
    return RS_MutableListIterator<RS_Entity*>(entities);
}



/**
 * @return The point which is closest to 'coord' 
 * (one of the vertexes)
 */
RS_Vector RS_EntityContainer::getNearestEndpoint(const RS_Vector& coord,
        double* dist) {

    double minDist = RS_MAXDOUBLE;  // minimum measured distance
    double curDist;                 // currently measured distance
    RS_Vector closestPoint(false);  // closest found endpoint
    RS_Vector point;                // endpoint found

    for (RS_Entity* en = firstEntity();
            en != NULL;
            en = nextEntity()) {

        if (en->isVisible()) {
            point = en->getNearestEndpoint(coord, &curDist);
            if (point.valid && curDist<minDist) {
                closestPoint = point;
                minDist = curDist;
                if (dist!=NULL) {
                    *dist = curDist;
                }
            }
        }
    }

    return closestPoint;
}



RS_Vector RS_EntityContainer::getNearestPointOnEntity(const RS_Vector& coord,
        bool onEntity, double* dist, RS_Entity** entity) {

    RS_Vector point(false);

    RS_Entity* e = getNearestEntity(coord, dist, RS2::ResolveNone);

    if (e!=NULL && e->isVisible()) {
        point = e->getNearestPointOnEntity(coord, onEntity, dist, entity);
    }

    return point;
}



RS_Vector RS_EntityContainer::getNearestCenter(const RS_Vector& coord,
        double* dist) {

    RS_Vector point(false);
    RS_Entity* closestEntity;

    //closestEntity = getNearestEntity(coord, NULL, RS2::ResolveAll);
    closestEntity = getNearestEntity(coord, NULL, RS2::ResolveNone);

    if (closestEntity!=NULL) {
        point = closestEntity->getNearestCenter(coord, dist);
    }

    return point;
}



RS_Vector RS_EntityContainer::getNearestMiddle(const RS_Vector& coord,
        double* dist) {

    RS_Vector point(false);
    RS_Entity* closestEntity;

    closestEntity = getNearestEntity(coord, NULL, RS2::ResolveNone);

    if (closestEntity!=NULL) {
        point = closestEntity->getNearestMiddle(coord, dist);
    }

    return point;


    /*
       double minDist = RS_MAXDOUBLE;  // minimum measured distance
       double curDist;                 // currently measured distance
       RS_Vector closestPoint;         // closest found endpoint
       RS_Vector point;                // endpoint found

       for (RS_Entity* en = firstEntity();
               en != NULL;
               en = nextEntity()) {

           if (en->isVisible()) {
               point = en->getNearestMiddle(coord, &curDist);
               if (curDist<minDist) {
                   closestPoint = point;
                   minDist = curDist;
                   if (dist!=NULL) {
                       *dist = curDist;
                   }
               }
           }
       }

       return closestPoint;
    */
}



RS_Vector RS_EntityContainer::getNearestDist(double distance,
        const RS_Vector& coord,
        double* dist) {

    RS_Vector point(false);
    RS_Entity* closestEntity;

    closestEntity = getNearestEntity(coord, NULL, RS2::ResolveNone);

    if (closestEntity!=NULL) {
        point = closestEntity->getNearestDist(distance, coord, dist);
    }

    return point;
}



/**
 * @return The intersection which is closest to 'coord' 
 */
RS_Vector RS_EntityContainer::getNearestIntersection(const RS_Vector& coord,
        double* dist) {

    double minDist = RS_MAXDOUBLE;  // minimum measured distance
    double curDist;                 // currently measured distance
    RS_Vector closestPoint(false); // closest found endpoint
    RS_Vector point;                // endpoint found
    RS_VectorList sol;
    RS_Entity* closestEntity;

    closestEntity = getNearestEntity(coord, NULL, RS2::ResolveAll);

    if (closestEntity!=NULL) {
        for (RS_Entity* en = firstEntity(RS2::ResolveAll);
                en != NULL;
                en = nextEntity(RS2::ResolveAll)) {

            if (en->isVisible() && en!=closestEntity) {
                sol = RS_Information::getIntersection(closestEntity,
                                                      en,
                                                      true);

                for (int i=0; i<4; i++) {
                    point = sol.get(i);
                    if (point.valid) {
                        curDist = coord.distanceTo(point);

                        if (curDist<minDist) {
                            closestPoint = point;
                            minDist = curDist;
                            if (dist!=NULL) {
                                *dist = curDist;
                            }
                        }
                    }
                }
            }
        }
        //}
    }

    return closestPoint;
}



RS_Vector RS_EntityContainer::getNearestRef(const RS_Vector& coord,
        double* dist, RS2::RefMode refMode) {

    double minDist = RS_MAXDOUBLE;  // minimum measured distance
    double curDist;                 // currently measured distance
    RS_Vector closestPoint(false);  // closest found endpoint
    RS_Vector point;                // endpoint found

    for (RS_Entity* en = firstEntity();
            en != NULL;
            en = nextEntity()) {

        if (en->isVisible()) {
            point = en->getNearestRef(coord, &curDist, refMode);
            if (point.valid && curDist<minDist) {
                closestPoint = point;
                minDist = curDist;
                if (dist!=NULL) {
                    *dist = curDist;
                }
            }
        }
    }

    return closestPoint;
}


RS_Vector RS_EntityContainer::getNearestSelectedRef(const RS_Vector& coord,
        double* dist, RS2::RefMode refMode) {

    RS_DEBUG->print("RS_EntityContainer::getNearestSelectedRef");

    double minDist = RS_MAXDOUBLE;  // minimum measured distance
    double curDist;                 // currently measured distance
    RS_Vector closestPoint(false);  // closest found endpoint
    RS_Vector point;                // endpoint found

    for (RS_Entity* en=firstEntity();
        en!=NULL;
        en=nextEntity()) {

        if (en->isVisible() && en->isSelected() && !en->isTemp()) {
            point = en->getNearestSelectedRef(coord, &curDist, refMode);
            if (point.valid && curDist<minDist) {
                closestPoint = point;
                minDist = curDist;
                if (dist!=NULL) {
                    *dist = curDist;
                }
            }
        }
    }

    return closestPoint;
}


double RS_EntityContainer::getDistanceToPoint(const RS_Vector& coord,
        RS_Entity** entity,
        RS2::ResolveLevel level,
        double solidDist,
        bool visually) {

    RS_DEBUG->print("RS_EntityContainer::getDistanceToPoint");

    double minDist = RS_MAXDOUBLE;      // minimum measured distance
    double curDist;                     // currently measured distance
    RS_Entity* closestEntity = NULL;    // closest entity found
    RS_Entity* subEntity = NULL;

    for (RS_Entity* e = firstEntity(level);
            e != NULL;
            e = nextEntity(level)) {

        if (e->isVisible()) {
            curDist = e->getDistanceToPoint(coord, &subEntity, level, solidDist, visually);

            if (curDist<minDist) {
                if (level!=RS2::ResolveAll) {
                    closestEntity = e;
                } else {
                    closestEntity = subEntity;
                }
                minDist = curDist;
            }
        }
    }

    if (entity!=NULL) {
        *entity = closestEntity;
    }
    
    RS_DEBUG->print("RS_EntityContainer::getDistanceToPoint: %f", minDist);

    return minDist;
}



RS_Entity* RS_EntityContainer::getNearestEntity(const RS_Vector& coord,
        double* dist,
        RS2::ResolveLevel level,
        bool visually) {

    RS_DEBUG->print("RS_EntityContainer::getNearestEntity");

    RS_Entity* e = NULL;

    // distance for points inside solids:
    double solidDist = RS_MAXDOUBLE;
    if (dist!=NULL) {
        solidDist = *dist;
    }

    double d = getDistanceToPoint(coord, &e, level, solidDist, visually);
    
    RS_DEBUG->print("RS_EntityContainer::getNearestEntity: dist: %f", d);

    if (e!=NULL && e->isVisible()==false) {
        e = NULL;
    }

    // if d is negative, use the default distance (used for points inside solids)
    if (dist!=NULL) {
        *dist = d;
    }
    RS_DEBUG->print("RS_EntityContainer::getNearestEntity: OK");

    return e;
}



/**
 * Rearranges the atomic entities in this container in a way that connected
 * entities are stored in the right order and direction.
 * Non-recoursive. Only affects atomic entities in this container.
 * 
 * @retval true all contours were closed
 * @retval false at least one contour is not closed
 */
bool RS_EntityContainer::optimizeContours() {

    RS_DEBUG->print("RS_EntityContainer::optimizeContours");

    RS_Vector current(false);
    RS_Vector start(false);
    RS_EntityContainer tmp;

    bool changed = false;
    bool closed = true;

    for (int ci=0; ci<count(); ++ci) {
        RS_Entity* e1=entityAt(ci);

        if (e1!=NULL && e1->isEdge() && !e1->isContainer() &&
                !e1->isProcessed()) {

            RS_AtomicEntity* ce = dynamic_cast<RS_AtomicEntity*>(e1);

            // next contour start:
            ce->setProcessed(true);
            tmp.addEntity(ce->clone());
            current = ce->getEndpoint();
            start = ce->getStartpoint();

            // find all connected entities:
            bool done;
            do {
                done = true;
                for (int ei=0; ei<count(); ++ei) {
                    RS_Entity* e2=entityAt(ei);

                    if (e2!=NULL && e2->isEdge() && !e2->isContainer() &&
                            !e2->isProcessed()) {

                        RS_AtomicEntity* e = dynamic_cast<RS_AtomicEntity*>(e2);

                        if (e->getStartpoint().distanceTo(current) <
                                1.0e-4) {

                            e->setProcessed(true);
                            tmp.addEntity(e->clone());
                            current = e->getEndpoint();

                            done=false;
                        } else if (e->getEndpoint().distanceTo(current) <
                                   1.0e-4) {

                            e->setProcessed(true);
                            RS_AtomicEntity* cl = dynamic_cast<RS_AtomicEntity*>(e->clone());
                            cl->reverse();
                            tmp.addEntity(cl);
                            current = cl->getEndpoint();

                            changed = true;
                            done=false;
                        }
                    }
                }
                if (!done) {
                    changed = true;
                }
            } while (!done);

            if (current.distanceTo(start)>1.0e-4) {
                closed = false;
            }
        }
    }

    // remove all atomic entities:
    bool done;
    do {
        done = true;
        for (RS_Entity* en=firstEntity(); en!=NULL; en=nextEntity()) {
            if (!en->isContainer()) {
                removeEntity(en);
                done = false;
                break;
            }
        }
    } while (!done);

    // add new sorted entities:
    for (RS_Entity* en=tmp.firstEntity(); en!=NULL; en=tmp.nextEntity()) {
        en->setProcessed(false);
        addEntity(en->clone());
    }

    RS_DEBUG->print("RS_EntityContainer::optimizeContours: OK");
    return closed;
}

    

/**
 * @return The orientation (cw, ccw) of the contour in this container.
 * This container is expected to contain exactly one closed contour
 * (e.g. a loop of a hatch).
 */
RS2::ContourOrientation RS_EntityContainer::getContourOrientation(bool* ok) {
    RS_Vector start(false);
    RS_Vector lastEnd(false);
    double area = 0.0;
                
    if (ok!=NULL) {
        *ok = true;
    }
    
    for (RS_Entity* e=firstEntity(); e!=NULL; e=nextEntity()) {
        if (e->isAtomic()) {
            RS_AtomicEntity* ae = dynamic_cast<RS_AtomicEntity*>(e);
    
            if (!lastEnd.valid || lastEnd.distanceTo(ae->getStartpoint()) < 1.0e-6) {
                if (!lastEnd.valid) {
                    start = ae->getStartpoint();
                }
                lastEnd = ae->getEndpoint();
                
                area += ae->getStartpoint().x * ae->getEndpoint().y - 
                        ae->getStartpoint().y * ae->getEndpoint().x;
            }
            else {
                if (ok!=NULL) {
                    RS_DEBUG->print("RS_EntityContainer::getContourOrientation: failed");
                    *ok = false;
                }
                break;
            }
        }
    }

    if (lastEnd.distanceTo(start) > 1.0e-6) {
        if (ok!=NULL) {
            RS_DEBUG->print("RS_EntityContainer::getContourOrientation: failed");
            *ok = false;
        }
    }

    RS_DEBUG->print("RS_EntityContainer::getContourOrientation: area: %f", area);

    if (area<0.0) {
        return RS2::ClockWise;
    }
    else {
        return RS2::CounterClockWise;
    }
}



bool RS_EntityContainer::hasEndpointsWithinWindow(const RS_Vector& v1, const RS_Vector& v2) {
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        if (e->hasEndpointsWithinWindow(v1, v2))  {
            return true;
        }
    }

    return false;
}
    
    

/**
 * Reverses every individual entity in this container. Does NOT
 * reverse the order of those entities!
 */
void RS_EntityContainer::reverse() {
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        if (e->isAtomic()) {
            dynamic_cast<RS_AtomicEntity*>(e)->reverse();
        }
        else if (e->isContainer()) {
            dynamic_cast<RS_EntityContainer*>(e)->reverse();
        }
    }
}



void RS_EntityContainer::reverseOrder() {
    RS_PtrList<RS_Entity> tmp;

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        tmp.prepend(e);
    }

    bool autoDel = entities.autoDelete();
    entities.setAutoDelete(false);
    entities.clear();
    entities.setAutoDelete(autoDel);

    for (RS_Entity* e=tmp.first(); e!=NULL; e=tmp.next()) {
        entities.append(e);
    }
}



void RS_EntityContainer::move(const RS_Vector& offset) {
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->move(offset);
    }
    if (autoUpdateBorders) {
        calculateBorders();
    }
}



void RS_EntityContainer::rotate(const RS_Vector& center, double angle) {
    RS_Entity* entityCursor = currentEntity();
    
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->rotate(center, angle);
    }
    if (autoUpdateBorders) {
        calculateBorders();
    }
    findEntity(entityCursor);
}



void RS_EntityContainer::scale(const RS_Vector& center, const RS_Vector& factor) {
    RS_DEBUG->print("RS_EntityContainer::scale");

    if (fabs(factor.x)>RS_TOLERANCE && fabs(factor.y)>RS_TOLERANCE) {
        RS_DEBUG->print("RS_EntityContainer::scale: scaling entities");
        for (RS_Entity* e=firstEntity(RS2::ResolveNone);
                e!=NULL;
                e=nextEntity(RS2::ResolveNone)) {
            e->scale(center, factor);
        }
    }
    if (autoUpdateBorders) {
        RS_DEBUG->print("RS_EntityContainer::scale: calculate borders");
        calculateBorders();
    }
    
    RS_DEBUG->print("RS_EntityContainer::scale: OK");
}



void RS_EntityContainer::mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2) {
    if (axisPoint1.distanceTo(axisPoint2)>1.0e-6) {
        for (RS_Entity* e=firstEntity(RS2::ResolveNone);
                e!=NULL;
                e=nextEntity(RS2::ResolveNone)) {
            e->mirror(axisPoint1, axisPoint2);
        }
    }
}


void RS_EntityContainer::stretch(const RS_Vector& firstCorner,
                                 const RS_Vector& secondCorner,
                                 const RS_Vector& offset) {

    if (getMin().isInWindow(firstCorner, secondCorner) &&
            getMax().isInWindow(firstCorner, secondCorner)) {

        move(offset);
    } else {
        for (RS_Entity* e=firstEntity(RS2::ResolveNone);
                e!=NULL;
                e=nextEntity(RS2::ResolveNone)) {
            e->stretch(firstCorner, secondCorner, offset);
        }
    }

    // some entitiy containers might need an update (e.g. RS_Leader):
    update();
}



void RS_EntityContainer::moveRef(const RS_Vector& ref,
                                 const RS_Vector& offset) {

    RS_DEBUG->print("RS_EntityContainer::moveRef");

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        if (!e->isUndone() && !e->getFlag(RS2::FlagTemp)) {
            e->moveRef(ref, offset);
        }
    }
    // 20080212:
    //update();
    if (autoUpdateBorders) {
        calculateBorders();
    }
}


void RS_EntityContainer::moveSelectedRef(const RS_Vector& ref,
        const RS_Vector& offset) {

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e=nextEntity(RS2::ResolveNone)) {
        e->moveSelectedRef(ref, offset);
    }
    if (autoUpdateBorders) {
        calculateBorders();
    }
}
    
    
    
void RS_EntityContainer::isoProject(RS2::IsoProjectionType type, double segmentLength) {

    for (int i=0; i<entities.count(); ++i) {
        RS_Entity* e = entities.at(i);

        if (e->rtti()==RS2::EntityArc) {
            RS_Arc* a = dynamic_cast<RS_Arc*>(e);
            RS_Polyline* pl = a->approximateWithLines(segmentLength);
            pl->isoProject(type, segmentLength);
            entities.replace(i, pl);
        }
        else if (e->rtti()==RS2::EntityCircle) {
            RS_Circle* c = dynamic_cast<RS_Circle*>(e);
            RS_Arc a(NULL, RS_ArcData(c->getCenter(), c->getRadius(), 0.0, 2*M_PI, false));
            a.copyPropertiesFrom(c);
            RS_Polyline* pl = a.approximateWithLines(segmentLength);
            pl->isoProject(type, segmentLength);
            entities.replace(i, pl);
        }
        else {
            e->isoProject(type, segmentLength);
        }
    }

    if (autoUpdateBorders) {
        calculateBorders();
    }
}



void RS_EntityContainer::draw(RS_Painter* painter, RS_GraphicView* view,
                              double /*patternOffset*/) {

    if (painter==NULL || view==NULL) {
        return;
    }

    RS_DEBUG->print("RS_EntityContainer::draw");

    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e = nextEntity(RS2::ResolveNone)) {

        view->drawEntity(e);
    }
    
    for (RS_Entity* e=firstEntity(RS2::ResolveNone);
            e!=NULL;
            e = nextEntity(RS2::ResolveNone)) {

        view->drawEntityHandles(e);
    }
    
    RS_DEBUG->print("RS_EntityContainer::draw: OK");
}


void RS_EntityContainer::changeProperty(const RS_String& propertyName, 
        const RS_Variant& propertyValue) {
        
    RS_Modification m(*this, NULL);
    m.changeProperty(propertyName, propertyValue);
}

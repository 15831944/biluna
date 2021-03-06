/****************************************************************************
** $Id: rs_entity.cpp 7935 2008-01-18 15:39:49Z andrew $
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


#include "rs_entity.h"

#include <iostream>

#include "rs_arc.h"
#include "rs_circle.h"
#include "rs_debug.h"
#include "rs_document.h"
#include "rs_ellipse.h"
#include "rs_graphic.h"
#include "rs_graphicview.h"
#include "rs_insert.h"
#include "rs_layer.h"
#include "rs_line.h"
#include "rs_painter.h"
#include "rs_point.h"
#include "rs_polyline.h"
#include "rs_text.h"
#include "rs_stringlist.h"

#ifdef RS_ARCH
#include "rs_archutils.h"
#endif


const char* RS_Entity::propertyType = QT_TRANSLATE_NOOP("QObject", "Type");
const char* RS_Entity::propertyLayer = QT_TRANSLATE_NOOP("QObject", "Layer");
#ifdef RS_ARCH
const char* RS_Entity::propertyFloor = QT_TRANSLATE_NOOP("QObject", "Floor");
#endif

/**
 * Default constructor.
 * @param parentContainer The parent entity of this entity.
 *               E.g. a line might have a graphic entity or
 *               a polyline entity as parent.
 */
RS_Entity::RS_Entity(RS_EntityContainer* parentContainer) {

    this->parentContainer = parentContainer;
    init();
}


/**
 * Destructor.
 */
RS_Entity::~RS_Entity() {}



/**
 * Initialisation. Called from all constructors.
 */
void RS_Entity::init() {
    resetBorders();

    setFlag(RS2::FlagVisible);
    //layer = NULL;
    //pen = RS_Pen();
    updateEnabled = true;
    setLayerToActive();
    setPenToActive();
    initId();
}

/**
 * Gives this entity a new unique id.
 */
void RS_Entity::initId() {
    static unsigned long int idCounter=0;
    id = idCounter++;
}
    


/**
 * Copies all basic entity properties from an existing entity.
 */
void RS_Entity::copyPropertiesFrom(RS_Entity* e) {
    parentContainer = e->parentContainer;
    minV = e->minV;
    maxV = e->maxV;
    layer = e->layer;
    pen = e->pen;
    updateEnabled = e->updateEnabled;
    RS_PropertyOwner::copyPropertiesFrom(e);
    setFlags(e->getFlags());
#ifdef RS_COMPAT
    varList = e->varList;
#endif
}



/**
 * Resets the borders of this element. 
 */
void RS_Entity::resetBorders() {
    // TODO: Check that. windoze XP crashes with MAXDOUBLE
    double maxd = RS_MAXDOUBLE;
    double mind = RS_MINDOUBLE;

    minV.set(maxd, maxd);
    maxV.set(mind, mind);
}



/**
 * Selects or deselects this entity.
 *
 * @param select True to select, false to deselect.
 * @param updateDependent Re-implementations might choose to 
 *   select dependent entities together with this. In some cases
 *   this is not desirable. updateDependent is False in these cases.
 * 
 * @return True on success, False if the layer is locked and select is True.
 */
bool RS_Entity::setSelected(bool select, bool /*updateDependent*/) {
    // layer is locked:
    if (select && isLocked()) {
        return false;
    }

    if (select) {
        setFlag(RS2::FlagSelected);
    } else {
        delFlag(RS2::FlagSelected);
    }

    setSelectionVisible(true);

    return true;
}



/**
 * Decides whether the selection of this entity is made visible to the user.
 * This is used for entities with dependent entities (e.g. walls with 
 * apertures) to make sure that selecting the entity also selects the 
 * dependent entities and yet only the properties of the selected entity
 * can be modified in the property editor.
 *
 * @param select True to make visible, false to make invisible.
 */
void RS_Entity::setSelectionVisible(bool on) {
    if (on) {
        setFlag(RS2::FlagSelectionVisible);
    } else {
        delFlag(RS2::FlagSelectionVisible);
    }
}



/**
 * Toggles select on this entity.
 */
bool RS_Entity::toggleSelected() {
    return setSelected(!isSelected());
}



/**
 * @return True if the entity is selected. Note that an entity might
 * not be selected but one of its parents is selected. In that case
 * this function returns false.
 */
bool RS_Entity::isSelected() const {
    return getFlag(RS2::FlagSelected);
}
    
    

/**
 * @return True if the entitiy's selection is made visible to the
 * user, false otherwise.
 */
bool RS_Entity::isSelectionVisible() const {
    return getFlag(RS2::FlagSelectionVisible);
}



/**
 * @return true if a parent entity of this entity is selected.
 */
bool RS_Entity::isParentContainerSelected() {
    RS_Entity* p = this;

    do {
        p = p->getParentContainer();
        if (p!=NULL && p->isSelected()==true) {
            return true;
        }
    } while(p!=NULL);

    return false;
}



/**
 * Gets the nearest reference point of this entity if it is selected. 
 * Containers re-implement this method to return the nearest reference
 * point of a selected sub entity.
 *
 * @param coord Coordinate (typically a mouse coordinate)
 * @param dist Pointer to a value which will contain the measured
 * distance between 'coord' and the closest point. The passed
 * pointer can also be NULL in which case the distance will be
 * lost.
 *
 * @return The closest point with the given distance to the endpoint.
 */
RS_Vector RS_Entity::getNearestSelectedRef(const RS_Vector& coord,
                                 double* dist,
                                 RS2::RefMode refMode) { 
    RS_DEBUG->print("RS_Entity::getNearestSelectedRef");

    if (isSelected()) {
        return getNearestRef(coord, dist, refMode);
    }
    else {
        return RS_Vector(false); 
    }
}



/**
 * Sets or resets the processed flag of this entity.
 *
 * @param on True to set, false to reset.
 */
void RS_Entity::setProcessed(bool on) {
    if (on) {
        setFlag(RS2::FlagProcessed);
    } else {
        delFlag(RS2::FlagProcessed);
    }
}



/**
 * @return True if the processed flag is set. 
 */
bool RS_Entity::isProcessed() const {
    return getFlag(RS2::FlagProcessed);
}



/**
 * @return True if the temp flag is set (for generated entities). 
 */
bool RS_Entity::isTemp() const {
    return getFlag(RS2::FlagTemp);
}


/**
 * Called when the undo state changed.
 *
 * @param undone true: entity has become invisible.
 *               false: entity has become visible.
 */
void RS_Entity::undoStateChanged(bool /*undone*/) {
    setSelected(false, false);
    update();
}


/**
 * @return true if this entity or any parent entities are undone.
 */
bool RS_Entity::isUndone() const {
    if (parentContainer==NULL) {
        return RS_Undoable::isUndone();
    }
    else {
        return RS_Undoable::isUndone() || parentContainer->isUndone();
    }
}


/**
 * @return True if the entity is in the given range.
 */
bool RS_Entity::isInWindow(const RS_Vector& v1, const RS_Vector& v2) {
    double right, left, top, bottom;

    right = std::max(v1.x, v2.x);
    left = std::min(v1.x, v2.x);
    top = std::max(v1.y, v2.y);
    bottom = std::min(v1.y, v2.y);

    return (getMin().x>=left &&
            getMax().x<=right &&
            getMin().y>=bottom &&
            getMax().y<=top);
}


/**
 * @param tolerance Tolerance.
 *
 * @retval true if the given point is on this entity.
 * @retval false otherwise
 */
bool RS_Entity::isPointOnEntity(const RS_Vector& coord,
                                double tolerance) {
    double dist = getDistanceToPoint(coord, NULL, RS2::ResolveNone);
    return (dist<=tolerance);
}


/**
 * Is this entity visible?
 *
 * @return true Only if the entity and the layer it is on are visible.
 * The Layer might also be NULL. In that case the layer visiblity 
* is ignored.
 */
bool RS_Entity::isVisible() {
    bool retLayer = false;
    bool retBlock = false;
        
    // entity is not visible:
    if (!getFlag(RS2::FlagVisible)) {
        return false;
    }
    
    // entity is in undo memory:
    if (isUndone()) {
        return false;
    }
    
    // invalid layer:
    if (getLayer()==NULL) {
        retLayer = true;
    }
    else {
        if (layer!=NULL) {
            if (!layer->isFrozen()) {
                retLayer = true;
            } else {
                return false;
            }
        }
        else {
            if (getLayer()==NULL) {
                retLayer = true;
            } else {
                if (!getLayer()->isFrozen()) {
                    retLayer = true;
                } else {
                    return false;
                }
            }
        }
    }
    
    if (getBlockOrInsert()==NULL) {
        retBlock = true;
    }
    else {
        if (getInsert()!=NULL && getInsert()->rtti()==RS2::EntityInsert) {
            RS_Insert* ins = (RS_Insert*)getInsert();
            RS_Block* block = ins->getBlockForInsert();
            if (block!=NULL && block->isFrozen()) {
                return false;
            }
            else {
                retBlock = true;
            }
        }

        if (getBlockOrInsert()->rtti()==RS2::EntityBlock) {
            if (getLayer(false)==NULL || !getLayer(false)->isFrozen()) {
                retBlock = true;
            } else {
                return false;
            }
        }
    
    
        if (getBlockOrInsert()->getLayer()==NULL) {
            retBlock = true;
        }
        else {
            if(getBlockOrInsert()->getLayer()->isFrozen()) {
                return false;
            }
            else {
                retBlock = true;
            }
        }
    }

    return (retBlock && retLayer);
}




/**
 * Sets the highlight status of the entity. Highlighted entities 
 * usually indicate a feedback to a user action.
 */
void RS_Entity::setHighlighted(bool on) {
    if (on) {
        setFlag(RS2::FlagHighlighted);
    } else {
        delFlag(RS2::FlagHighlighted);
    }
}



/**
 * @return true if the entity is highlighted.
 */
bool RS_Entity::isHighlighted() {
    return getFlag(RS2::FlagHighlighted);
}



/**
 * @return true if the layer this entity is on is locked.
 */
bool RS_Entity::isLocked() {
    if (getLayer(true)!=NULL && getLayer()->isLocked()) {
        return true;
    } else {
        return false;
    }
}


/**
 * @return The parent graphic in which this entity is stored
 * or the parent's parent graphic or NULL if none of the parents
 * are stored in a graphic.
 */
RS_Graphic* RS_Entity::getGraphic() {
    if (rtti()==RS2::EntityGraphic) {
        return dynamic_cast<RS_Graphic*>(this);
    } else if (parentContainer==NULL) {
        return NULL;
    } else {
        return parentContainer->getGraphic();
    }
}



/**
 * @return The parent block in which this entity is stored
 * or the parent's parent block or NULL if none of the parents
 * are stored in a block.
 */
RS_Block* RS_Entity::getBlock() {
    if (rtti()==RS2::EntityBlock) {
        return dynamic_cast<RS_Block*>(this);
    } else if (parentContainer==NULL) {
        return NULL;
    } else {
        return parentContainer->getBlock();
    }
}



/**
 * @return The parent insert in which this entity is stored
 * or the parent's parent block or NULL if none of the parents
 * are stored in a block.
 */
RS_Insert* RS_Entity::getInsert() {
    if (rtti()==RS2::EntityInsert) {
        return dynamic_cast<RS_Insert*>(this);
    } else if (parentContainer==NULL) {
        return NULL;
    } else {
        return parentContainer->getInsert();
    }
}



/**
 * @return The parent block or insert in which this entity is stored
 * or the parent's parent block or insert or NULL if none of the parents
 * are stored in a block or insert.
 */
RS_Entity* RS_Entity::getBlockOrInsert() {
    if (rtti()==RS2::EntityBlock || rtti()==RS2::EntityInsert) {
        return this;
    } else if (parentContainer==NULL) {
        return NULL;
    } else {
        return parentContainer->getBlockOrInsert();
    }
}



/**
 * @return The parent document in which this entity is stored
 * or the parent's parent document or NULL if none of the parents
 * are stored in a document. Note that a document is usually 
 * either a Graphic or a Block.
 */
RS_Document* RS_Entity::getDocument() {
    if (isDocument()==true) {
        return dynamic_cast<RS_Document*>(this);
    } else if (parentContainer==NULL) {
        return NULL;
    } else {
        return parentContainer->getDocument();
    }
}



/**
 * Sets a variable value for the parent graphic object.
 *
 * @param key Variable name (e.g. "$DIMASZ")
 * @param val Default value
 */
void RS_Entity::addGraphicVariable(const RS_String& key, double val, int code) {
    RS_Graphic* graphic = getGraphic();
    if (graphic!=NULL) {
        graphic->addVariable(key, val, code);
    }
}



/**
 * Sets a variable value for the parent graphic object.
 *
 * @param key Variable name (e.g. "$DIMASZ")
 * @param val Default value
 */
void RS_Entity::addGraphicVariable(const RS_String& key, int val, int code) {
    RS_Graphic* graphic = getGraphic();
    if (graphic!=NULL) {
        graphic->addVariable(key, val, code);
    }
}



/**
 * Sets a variable value for the parent graphic object.
 *
 * @param key Variable name (e.g. "$DIMASZ")
 * @param val Default value
 */
void RS_Entity::addGraphicVariable(const RS_String& key,
                                   const RS_String& val, int code) {
    RS_Graphic* graphic = getGraphic();
    if (graphic!=NULL) {
        graphic->addVariable(key, val, code);
    }
}



/**
 * A safe member function to return the given variable.
 *
 * @param key Variable name (e.g. "$DIMASZ")
 * @param def Default value
 *
 * @return value of variable or default value if the given variable
 *    doesn't exist.
 */
double RS_Entity::getGraphicVariableDouble(const RS_String& key, double def) {
    RS_Graphic* graphic = getGraphic();
    double ret=def;
    if (graphic!=NULL) {
        ret = graphic->getVariableDouble(key, def);
    }
    return ret;
}



/**
 * A safe member function to return the given variable.
 *
 * @param key Variable name (e.g. "$DIMASZ")
 * @param def Default value
 *
 * @return value of variable or default value if the given variable
 *    doesn't exist.
 */
int RS_Entity::getGraphicVariableInt(const RS_String& key, int def) {
    RS_Graphic* graphic = getGraphic();
    int ret=def;
    if (graphic!=NULL) {
        ret = graphic->getVariableInt(key, def);
    }
    return ret;
}



/**
 * A safe member function to return the given variable.
 *
 * @param key Variable name (e.g. "$DIMASZ")
 * @param def Default value
 *
 * @return value of variable or default value if the given variable
 *    doesn't exist.
 */
RS_String RS_Entity::getGraphicVariableString(const RS_String& key,
        const RS_String&  def) {
    RS_Graphic* graphic = getGraphic();
    RS_String ret=def;
    if (graphic!=NULL) {
        ret = graphic->getVariableString(key, def);
    }
    return ret;
}



/**
 * @return The unit the parent graphic works on or None if there's no
 * parent graphic.
 */
RS2::Unit RS_Entity::getGraphicUnit() {
    RS_Graphic* graphic = getGraphic();
    RS2::Unit ret = RS2::None;
    if (graphic!=NULL) {
        ret = graphic->getUnit();
    }
    return ret;
}



/**
 * Returns a pointer to the layer this entity is on or NULL.
 *
 * @para resolve true: if the layer is ByBlock, the layer of the 
 *               block this entity is in is returned. 
 *               false: the layer of the entity is returned.
 *
 * @return pointer to the layer this entity is on. If the layer
 * is set to NULL the layer of the next parent that is not on
 * layer NULL is returned. If all parents are on layer NULL, NULL
 * is returned.
 */
RS_Layer* RS_Entity::getLayer(bool resolve) const {
    if (resolve) {
        // we have no layer but a parent that might have one.
        // return parent's layer instead:
        if (layer==NULL /*|| layer->getName()=="ByBlock"*/) {
            if (parentContainer!=NULL) {
                return parentContainer->getLayer(true);
            } else {
                return NULL;
            }
        }
    }

    // return our layer. might still be NULL:
    return layer;
}



/**
 * Sets the layer of this entity to the layer with the given name 
 */
void RS_Entity::setLayer(const RS_String& name) {
    RS_DEBUG->print("RS_Entity::setLayer: %s", (const char*)name.toLatin1());

    RS_Graphic* graphic = getGraphic();
    if (graphic!=NULL) {
        layer = graphic->findLayer(name);
    } else {
        RS_DEBUG->print("RS_Entity::setLayer: graphic is NULL");
        layer = NULL;
    }
}



/**
 * Sets the layer of this entity to the layer given.
 */
void RS_Entity::setLayer(RS_Layer* l) {
    layer = l;
}



/**
 * Sets the layer of this entity to the current layer of
 * the graphic this entity is in. If this entity (and none
 * of its parents) are in a graphic the layer is set to NULL.
 */
void RS_Entity::setLayerToActive() {
    RS_Graphic* graphic = getGraphic();

    if (graphic!=NULL) {
        layer = graphic->getActiveLayer();
    } else {
        layer = NULL;
    }
}



/**
 * Gets the pen needed to draw this entity.
 * The attributes can also come from the layer this entity is on
 * if the flags are set accordingly.
 * 
 * @param resolve true: Resolve the pen to a drawable pen (e.g. the pen 
 *         from the layer or parent..)
 *         false: Don't resolve and return a pen or ByLayer, ByBlock, ...
 *
 * @return Pen for this entity. 
 */
RS_Pen RS_Entity::getPen(bool resolve) const {

    if (!resolve) {
        return pen;
    } else {

        RS_Pen p = pen;
        RS_Layer* l = getLayer(true);

        // use parental attributes (e.g. vertex of a polyline, block
        // entities when they are drawn in block documents):
        if (!p.isValid() || p.getColor().isByBlock()) {
            if (parentContainer!=NULL) {
                p = parentContainer->getPen();
            }
        }
        // use layer attributes:
        else if (l!=NULL) {

            // use layer's color:
            if (p.getColor().isByLayer()) {
                p.setColor(l->getPen().getColor());
            }

            // use layer's width:
            if (p.getWidth()==RS2::WidthByLayer) {
                p.setWidth(l->getPen().getWidth());
            }

            // use layer's linetype:
            if (p.getLineType()==RS2::LineByLayer) {
                p.setLineType(l->getPen().getLineType());
            }
        }

        return p;
    }
}



/**
 * Sets the pen of this entity to the current pen of
 * the graphic this entity is in. If this entity (and none
 * of its parents) are in a graphic the pen is not changed.
 */
void RS_Entity::setPenToActive() {
    RS_Document* doc = getDocument();
    if (doc!=NULL) {
        pen = doc->getActivePen();
    }
}



/**
 * Implementations must stretch the given range of the entity 
 * by the given offset. This default implementation moves the 
 * whole entity if it is completely inside the given range.
 */
void RS_Entity::stretch(const RS_Vector& firstCorner,
                        const RS_Vector& secondCorner,
                        const RS_Vector& offset) {

    if (getMin().isInWindow(firstCorner, secondCorner) &&
            getMax().isInWindow(firstCorner, secondCorner)) {

        move(offset);
    }
}



/**
 * @return Factor for scaling the line styles considering the current 
 * paper scaling and the fact that styles are stored in Millimeter.
 */
double RS_Entity::getStyleFactor(RS_GraphicView* view) {
    double styleFactor = 1.0;

    if (view!=NULL) {
        int penWidth = (int)getPen(true).getWidth();
        
        // draft mode or zero width: no line pattern scaling
        if (penWidth==0 || 
            (view->isPrinting()==false && view->isDraftMode())) {

            styleFactor = 1.0/view->getFactor().x;
        } else {
            // the factor caused by the unit:
            RS2::Unit unit = RS2::None;
            RS_Graphic* g = getGraphic();
            if (g!=NULL) {
                unit = g->getUnit();
                styleFactor = RS_Units::convert(1.0, RS2::Millimeter, unit);
            }

            // the factor caused by the line width:
            if (penWidth>0) {
                styleFactor *= ((double)penWidth/100.0);
            } 
        }

        if (penWidth>0) {
            if (view->isPrinting() || view->isPrintPreview() || view->isDraftMode()==false) {
                RS_Graphic* graphic = getGraphic();
                if (graphic!=NULL && graphic->getPaperScale()>1.0e-9) {
                    styleFactor /= graphic->getPaperScale();
                }
            }
        }
    }

    //RS_DEBUG->print("RS_Entity::getStyleFactor: stylefactor: %f", styleFactor);
    //RS_DEBUG->print("RS_Entity::getStyleFactor: viewfactor: %f", view->getFactor().x);

    if (!view->isPrinting() && styleFactor*view->getFactor().x<0.2) {
        styleFactor = -1.0;
    }

    return styleFactor;
}

/**
 * @return User defined variable connected to this entity.
 *
 * OBSOLETE since QCad 2.1.1
 */
#ifdef RS_COMPAT
RS_String* RS_Entity::getUserDefVar(const RS_String& key) {
    if (varList.contains(key)) {
        return &varList[key];
    }
    else {
        return NULL;
    }
}
#endif


/**
 * Add a user defined variable to this entity.
 *
 * OBSOLETE since QCad 2.1.1
 */
#ifdef RS_COMPAT
void RS_Entity::setUserDefVar(const RS_String& key, const RS_String& val) {
    varList.insert(key, val);
}
#endif



/**
 * Deletes the given user defined variable.
 *
 * OBSOLETE since QCad 2.1.1
 */
#ifdef RS_COMPAT
void RS_Entity::delUserDefVar(const RS_String& key) {
    varList.remove(key);
}
#endif



/**
 * @return A list of all keys connected to this entity.
 *
 * OBSOLETE since QCad 2.1.1
 */
#ifdef RS_COMPAT
RS_StringList RS_Entity::getAllKeys() {
    return varList.keys();
}
#endif



/**
 * Supports custom properties and layer properties.
 * Ignores the property for the entity type that cannot be changed.
 */    
void RS_Entity::setProperty(const RS_String& name, const RS_Variant& value) {
    if (name==RS_ENTITY_LAYER) {
        setLayer(value.toString());
    }
#ifdef RS_ARCH
    else if (name==RS_ENTITY_FLOOR) {
        setLayer(RS_ArchUtils::floorToLayerName(value.toString()));
    }
#endif
    else if (name!=RS_ENTITY_TYPE) {
        RS_PropertyOwner::setProperty(name, value);
    }
}



/**
 * Returns the property with the given name or 'def'.
 *
 * @param name Property name.
 * @param def Default value to return if the given property is undefined.
 */
RS_Variant RS_Entity::getProperty(const RS_String& name, const RS_Variant& def) {
    if (name==RS_ENTITY_TYPE) {
        return typeName();
    }
    else if (name==RS_ENTITY_LAYER) {
        RS_Layer* layer = getLayer();
        if (layer!=NULL) {
#ifdef RS_ARCH
            if (!RS_ArchUtils::isFloor(layer->getName())) {
                return layer->getName();
            }
            else {
                return def;
            }
#else
            return layer->getName();
#endif
        }
        else {
            return def;
        }
    }
#ifdef RS_ARCH
    else if (name==RS_ENTITY_FLOOR) {
        RS_Layer* layer = getLayer();
        if (layer!=NULL) {
            return RS_ArchUtils::layerToFloorName(layer->getName());
        }
        else {
            return def;
        }
    }
#endif
    else {
        return RS_PropertyOwner::getProperty(name, def);
    }
}



/**
 * @return Names of all known properties that are defined for this entity.
 */
RS_StringList RS_Entity::getPropertyNames(bool /*includeGeometry*/) {
    RS_StringList ret; // = RS_PropertyOwner::getPropertyNames();
    ret << RS_ENTITY_TYPE;
    if (rtti()!=RS2::EntityGraphic) {
        ret << RS_ENTITY_LAYER;
#ifdef RS_ARCH
        ret << RS_ENTITY_FLOOR;
#endif
    }
    return ret + RS_PropertyOwner::getPropertyNames();
}



RS_PropertyAttributes RS_Entity::getPropertyAttributes(
    const RS_String& propertyName) {

    RS_PropertyAttributes ret;

#ifdef RS_ARCH
    if (propertyName==RS_ENTITY_FLOOR ||
        propertyName==RS_ENTITY_LAYER) {
        ret.affectsOtherProperties = true;
    }
#endif
    
    // only one choice for the entity type:
    if (propertyName==RS_ENTITY_TYPE) {
        ret.choices << getProperty(propertyName).toString();
    }
    // layer choice:
    else if (propertyName==RS_ENTITY_LAYER) {
        RS_Graphic* graphic = getGraphic();
        if (graphic!=NULL) {
            RS_LayerList* layerList = graphic->getLayerList();
            for (int i=0; i<layerList->count(); i++) {
#ifdef RS_ARCH
                if (!RS_ArchUtils::isFloor(layerList->at(i)->getName())) {
                    ret.choices << layerList->at(i)->getName();
                }
#else
                ret.choices << layerList->at(i)->getName();
#endif
            }
        }
    }
#ifdef RS_ARCH
    // floor choice:
    else if (propertyName==RS_ENTITY_FLOOR) {
        RS_Graphic* graphic = getGraphic();
        if (graphic!=NULL) {
            RS_LayerList* layerList = graphic->getLayerList();
            for (int i=0; i<layerList->count(); i++) {
                if (RS_ArchUtils::isFloor(layerList->at(i)->getName())) {
                    ret.choices << RS_ArchUtils::layerToFloorName(layerList->at(i)->getName());
                }
            }
        }
    }
#endif

    return ret;
}


/**
 * @return the given graphic variable or the default value given in mm 
 * converted to the graphic unit.
 * If the variable is not found it is added with the given default
 * value converted to the local unit.
 */
double RS_Entity::getGraphicVariable(const RS_String& key, double defMM,
                                        int code) {

    double v = getGraphicVariableDouble(key, RS_MINDOUBLE);
    if (v<=RS_MINDOUBLE) {
        addGraphicVariable(
            key,
            RS_Units::convert(defMM, RS2::Millimeter, getGraphicUnit()),
            code);
        v = getGraphicVariableDouble(key, 1.0);
    }

    return v;
}

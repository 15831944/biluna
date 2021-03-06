/****************************************************************************
** $Id: rs_insert.cpp 9471 2008-03-28 11:12:04Z andrew $
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


#include "rs_insert.h"

#include "rs_block.h"
#include "rs_graphic.h"
#include "rs_settings.h"


const char* RS_Insert::propertyName = QT_TRANSLATE_NOOP("QObject", "Block Name");
const char* RS_Insert::propertyInsertionPointX = QT_TRANSLATE_NOOP("QObject", "Position|X");
const char* RS_Insert::propertyInsertionPointY = QT_TRANSLATE_NOOP("QObject", "Position|Y");
const char* RS_Insert::propertyScaleFactorX = QT_TRANSLATE_NOOP("QObject", "Scale|X");
const char* RS_Insert::propertyScaleFactorY = QT_TRANSLATE_NOOP("QObject", "Scale|Y");
const char* RS_Insert::propertyAngle = QT_TRANSLATE_NOOP("QObject", "Angle");
const char* RS_Insert::propertyCols = QT_TRANSLATE_NOOP("QObject", "Columns");
const char* RS_Insert::propertyRows = QT_TRANSLATE_NOOP("QObject", "Rows");
const char* RS_Insert::propertySpacingX = QT_TRANSLATE_NOOP("QObject", "Spacing|X");
const char* RS_Insert::propertySpacingY = QT_TRANSLATE_NOOP("QObject", "Spacing|Y");



/**
 * @param parentContainer The graphic this block belongs to.
 */
RS_Insert::RS_Insert(RS_EntityContainer* parentContainer,
                     const RS_InsertData& d)
        : RS_Entity(parentContainer), RS_EntityContainer(parentContainer), data(d) {

    block = NULL;

    if (data.updateMode!=RS2::NoUpdate) {
        update();
        //calculateBorders();
    }
}


/**
 * Destructor.
 */
RS_Insert::~RS_Insert() {
    RS_DEBUG->print("RS_Insert::~RS_Insert");
}


/**
 * Updates the entity buffer of this insert entity. This method
 * needs to be called whenever the block this insert is based on changes.
 */
void RS_Insert::update() {
    
    if (updateEnabled==false) {
        return;
    }

    /*
    RS_DEBUG->print("RS_Insert::update");
    RS_DEBUG->print("RS_Insert::update: name: %s", (const char*)data.name.toLatin1());
    RS_DEBUG->print("RS_Insert::update: insertionPoint: %f/%f", 
        data.insertionPoint.x, data.insertionPoint.y);
    */


    clear();

    RS_Block* blk = getBlockForInsert();
    if (blk==NULL) {
        //return NULL;
        RS_DEBUG->print("RS_Insert::update: Block is NULL");
        return;
    }

    if (isUndone()) {
        RS_DEBUG->print("RS_Insert::update: Insert is in undo list");
        return;
    }

    if (fabs(data.scaleFactor.x)<1.0e-6 || fabs(data.scaleFactor.y)<1.0e-6) {
        RS_DEBUG->print("RS_Insert::update: scale factor is 0");
        return;
    }
    
    RS_Pen tmpPen;

    /*RS_MutableListIterator<RS_Entity> it = createIterator();
    RS_Entity* e;
    while ( (e = it.current()) != NULL ) {
        ++it;*/

    RS_DEBUG->print("RS_Insert::update: cols: %d, rows: %d", 
        data.cols, data.rows);
    RS_DEBUG->print("RS_Insert::update: block has %d entities", 
        blk->count());
    
    int max = 0;
    int pCounter = 0;
    if (data.updateMode==RS2::PreviewUpdate) {
        RS_SETTINGS->beginGroup("/Appearance");
        max = RS_SETTINGS->readNumEntry("/MaxPreview", 100);
        RS_SETTINGS->endGroup();
    }
    
    for (RS_Entity* e=blk->firstEntity(); e!=NULL; e=blk->nextEntity()) {
        for (int c=0; c<data.cols; ++c) {
            for (int r=0; r<data.rows; ++r) {
            
                if (data.updateMode==RS2::PreviewUpdate) {
                    if (pCounter>max) {
                        break;
                    }
                    else {
                        pCounter++;
                    }
                }

                if (e->rtti()==RS2::EntityInsert) {
                    RS_DEBUG->print("RS_Insert::update: updating sub-insert");
                    dynamic_cast<RS_Insert*>(e)->update();
                }
                
                RS_Entity* ne = e->clone();
                ne->initId();
                ne->setUpdateEnabled(false);
                ne->setParentContainer(this);
                ne->setVisible(getFlag(RS2::FlagVisible));
                
                // Move:
                if (fabs(data.scaleFactor.x)>1.0e-6 && 
                    fabs(data.scaleFactor.y)>1.0e-6) {
                    ne->move(data.insertionPoint +
                             RS_Vector(data.spacing.x/data.scaleFactor.x*c,
                                       data.spacing.y/data.scaleFactor.y*r));
                }
                else {
                    ne->move(data.insertionPoint);
                }
                // Move because of block base point:
                ne->move(blk->getBasePoint()*-1);
                // Scale:
                ne->scale(data.insertionPoint, data.scaleFactor);
                // Rotate:
                ne->rotate(data.insertionPoint, data.angle);
                // Select:
                ne->setSelected(isSelected());

                // individual entities can be on indiv. layers
                tmpPen = ne->getPen(false);

                // color from block (free floating):
                if (tmpPen.getColor()==RS_Color(RS2::FlagByBlock)) {
                    tmpPen.setColor(getPen().getColor());
                }

                // line width from block (free floating):
                if (tmpPen.getWidth()==RS2::WidthByBlock) {
                    tmpPen.setWidth(getPen().getWidth());
                }

                // line type from block (free floating):
                if (tmpPen.getLineType()==RS2::LineByBlock) {
                    tmpPen.setLineType(getPen().getLineType());
                }

                // now that we've evaluated all flags, let's strip them:
                // TODO: strip all flags (width, line type)
                //tmpPen.setColor(tmpPen.getColor().stripFlags());

                ne->setPen(tmpPen);
                
                ne->setUpdateEnabled(true);
                
                ne->update();

                addEntity(ne);
                
            }
        }
    }
    calculateBorders();

    RS_DEBUG->print("RS_Insert::update: OK");
}



/**
 * @return Pointer to the block associated with this Insert or
 *   NULL if the block couldn't be found. Blocks are requested
 *   from the blockSource if one was supplied and otherwise from 
 *   the closest parent graphic.
 */
RS_Block* RS_Insert::getBlockForInsert() {
    if (block!=NULL) {
        return block;
    }

    RS_BlockList* blkList;

    if (data.blockSource==NULL) {
        if (getGraphic()!=NULL) {
            blkList = getGraphic()->getBlockList();
            RS_DEBUG->print("RS_Insert::getBlockForInsert: "
                "got block list from graphic");
        } else {
            blkList = NULL;
            RS_DEBUG->print(RS_Debug::D_WARNING, 
                "RS_Insert::getBlockForInsert: "
                "cannot find a graphic to get the block list");
        }
    } else {
        blkList = data.blockSource;
        RS_DEBUG->print("RS_Insert::getBlockForInsert: "
           "got block list from explicit block source");
    }

    RS_Block* blk = NULL;
    if (blkList!=NULL) {
        blk = blkList->find(data.name);
    }
    else {
        RS_DEBUG->print(RS_Debug::D_WARNING, 
            "RS_Insert::getBlockForInsert: "
            "block list is NULL");
    }

    block = blk;

    return blk;
}


/**
 * Is this insert visible? (re-implementation from RS_Entity)
 *
 * @return true Only if the entity and the block and the layer it is on 
 * are visible.
 * The Layer might also be NULL. In that case the layer visiblity 
 * is ignored.
 * The Block might also be NULL. In that case the block visiblity 
 * is ignored.
 */
bool RS_Insert::isVisible() {
    RS_Block* blk = getBlockForInsert();
    if (blk!=NULL) {
        if (blk->isFrozen()) {
            return false;
        }
    }

    return RS_Entity::isVisible();
}


RS_VectorList RS_Insert::getRefPoints(RS2::RefMode /*refMode*/) {
    RS_VectorList ret(data.insertionPoint);
    return ret;
}
    


RS_Vector RS_Insert::getNearestRef(const RS_Vector& coord,
                                     double* dist,
                                     RS2::RefMode /*refMode*/) {

    return getRefPoints().getClosest(coord, dist);
}



void RS_Insert::move(const RS_Vector& offset) {
    data.insertionPoint.move(offset);
    update();
}



void RS_Insert::rotate(const RS_Vector& center, double angle) {
    data.insertionPoint.rotate(center, angle);
    data.angle = RS_Math::correctAngle(data.angle+angle);
    update();
}



void RS_Insert::scale(const RS_Vector& center, const RS_Vector& factor) {
    data.insertionPoint.scale(center, factor);
    data.scaleFactor.scale(RS_Vector(0.0, 0.0), factor);
    data.spacing.scale(RS_Vector(0.0, 0.0), factor);
    update();
}



void RS_Insert::mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2) {
    data.insertionPoint.mirror(axisPoint1, axisPoint2);
    
    RS_Vector vec;
    vec.setPolar(1.0, data.angle);
    vec.mirror(RS_Vector(0.0,0.0), axisPoint2-axisPoint1);
    data.angle = vec.angle();

    data.scaleFactor.y*=-1;

    update();
}



void RS_Insert::moveRef(const RS_Vector& ref, const RS_Vector& offset) {
    if (ref.distanceTo(data.insertionPoint)<1.0e-4) {
        data.insertionPoint += offset;
    } 
    calculateBorders();
}



RS_Variant RS_Insert::getProperty(const RS_String& name, const RS_Variant& def) {
    if (name==RS_INSERT_NAME) {
        return data.name;
    } else if (name==RS_INSERT_INSERTIONPOINT_X) {
        return data.insertionPoint.x;
    } else if (name==RS_INSERT_INSERTIONPOINT_Y) {
        return data.insertionPoint.y;
    } else if (name==RS_INSERT_SCALEFACTOR_X) {
        return data.scaleFactor.x;
    } else if (name==RS_INSERT_SCALEFACTOR_Y) {
        return data.scaleFactor.y;
    } else if (name==RS_INSERT_ANGLE) {
        return RS_Math::rad2deg(data.angle);
    } else if (name==RS_INSERT_COLS) {
        return data.cols;
    } else if (name==RS_INSERT_ROWS) {
        return data.rows;
    } else if (name==RS_INSERT_SPACING_X) {
        return data.spacing.x;
    } else if (name==RS_INSERT_SPACING_Y) {
        return data.spacing.y;
    } else {
        return RS_EntityContainer::getProperty(name, def);
    }
}



RS_StringList RS_Insert::getPropertyNames(bool includeGeometry) {
    RS_StringList ret = RS_EntityContainer::getPropertyNames(includeGeometry);
    ret << RS_INSERT_NAME;
    if (includeGeometry) {
        ret << RS_INSERT_INSERTIONPOINT_X;
        ret << RS_INSERT_INSERTIONPOINT_Y;
        ret << RS_INSERT_SCALEFACTOR_X;
        ret << RS_INSERT_SCALEFACTOR_Y;
        ret << RS_INSERT_ANGLE;
        ret << RS_INSERT_COLS;
        ret << RS_INSERT_ROWS;
        ret << RS_INSERT_SPACING_X;
        ret << RS_INSERT_SPACING_Y;
    }
    return ret;
}


void RS_Insert::setProperty(const RS_String& name, const RS_Variant& value) {
    if (name==RS_INSERT_NAME) {
        data.name = value.toString();
    } else if (name==RS_INSERT_INSERTIONPOINT_X) {
        data.insertionPoint.x = RS_Math::eval(value.toString());
    } else if (name==RS_INSERT_INSERTIONPOINT_Y) {
        data.insertionPoint.y = RS_Math::eval(value.toString());
    } else if (name==RS_INSERT_SCALEFACTOR_X) {
        data.scaleFactor.x = RS_Math::eval(value.toString());
    } else if (name==RS_INSERT_SCALEFACTOR_Y) {
        data.scaleFactor.y = RS_Math::eval(value.toString());
    } else if (name==RS_INSERT_ANGLE) {
        data.angle = RS_Math::deg2rad(RS_Math::eval(value.toString()));
    } else if (name==RS_INSERT_COLS) {
        data.cols = value.toInt();
    } else if (name==RS_INSERT_ROWS) {
        data.rows = value.toInt();
    } else if (name==RS_INSERT_SPACING_X) {
        data.spacing.x = RS_Math::eval(value.toString());
    } else if (name==RS_INSERT_SPACING_Y) {
        data.spacing.y = RS_Math::eval(value.toString());
    } else {
        RS_EntityContainer::setProperty(name, value);
    }
    update();
}

/****************************************************************************
** $Id: rs_dimdiametric.cpp 7930 2008-01-17 21:23:04Z andrew $
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


#include "rs_dimdiametric.h"
#include "rs_solid.h"
#include "rs_graphic.h"
#include "rs_units.h"

const char* RS_DimDiametric::propertyDefinitionPoint1X = QT_TRANSLATE_NOOP("QObject", "Point 1|X");
const char* RS_DimDiametric::propertyDefinitionPoint1Y = QT_TRANSLATE_NOOP("QObject", "Point 1|Y");
const char* RS_DimDiametric::propertyDefinitionPoint2X = QT_TRANSLATE_NOOP("QObject", "Point 2|X");
const char* RS_DimDiametric::propertyDefinitionPoint2Y = QT_TRANSLATE_NOOP("QObject", "Point 2|Y");
const char* RS_DimDiametric::propertyLeader = QT_TRANSLATE_NOOP("QObject", "Leader");

/**
 * Constructor.
 *
 * @para parentContainer Parent Entity Container.
 * @para d Common dimension geometrical data.
 * @para ed Extended geometrical data for diametric dimension.
 */
RS_DimDiametric::RS_DimDiametric(RS_EntityContainer* parentContainer,
                           const RS_DimensionData& d,
                           const RS_DimDiametricData& ed)
        : RS_Entity(parentContainer), RS_Dimension(parentContainer, d), edata(ed) {

    calculateBorders();
}



/**
 * @return Automatically created label for the default 
 * measurement of this dimension.
 */
RS_String RS_DimDiametric::getMeasuredLabel() {

    // Definitive dimension line:
    double dist = data.definitionPoint.distanceTo(edata.definitionPoint);

    RS_Graphic* graphic = getGraphic();

    RS_String ret;
    if (graphic!=NULL) {
        ret = RS_Units::formatLinear(dist, graphic->getUnit(), 
            graphic->getLinearFormat(), graphic->getLinearPrecision(),
            false,
            graphic->showLeadingZeroes(), graphic->showTrailingZeroes());
    }
    else {
        ret = RS_String("%1").arg(dist);
    }

    return ret;
}



RS_VectorList RS_DimDiametric::getRefPoints(RS2::RefMode /*refMode*/) {
    RS_VectorList ret;
    ret << edata.definitionPoint;
    ret << data.definitionPoint;
    if (data.middleOfText.valid) {
        ret << data.middleOfText;
    }
    else {
        ret << autoTextPos;
    }
    return ret;
}


/**
 * Updates the sub entities of this dimension. Called when the 
 * dimension or the position, alignment, .. changes.
 *
 * @param autoText Automatically reposition the text label
 */
void RS_DimDiametric::update(bool autoText) {

    RS_DEBUG->print("RS_DimDiametric::update");

    clear();

    if (isUndone()) {
        return;
    }

    // dimension line:
    updateCreateDimensionLine(data.definitionPoint, edata.definitionPoint,
        true, true, autoText);

    calculateBorders();
}



void RS_DimDiametric::move(const RS_Vector& offset) {
    RS_Dimension::move(offset);

    edata.definitionPoint.move(offset);
    update();
}



void RS_DimDiametric::rotate(const RS_Vector& center, double angle) {
    RS_Dimension::rotate(center, angle);

    edata.definitionPoint.rotate(center, angle);
    update();
}



void RS_DimDiametric::scale(const RS_Vector& center, const RS_Vector& factor) {
    RS_Dimension::scale(center, factor);

    edata.definitionPoint.scale(center, factor);
    edata.leader*=factor.x;
    update();
}



void RS_DimDiametric::mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2) {
    RS_Dimension::mirror(axisPoint1, axisPoint2);

    edata.definitionPoint.mirror(axisPoint1, axisPoint2);
    update();
}



void RS_DimDiametric::moveRef(const RS_Vector& ref, const RS_Vector& offset) {

    if (ref.distanceTo(edata.definitionPoint)<1.0e-4) {
        RS_Vector c = (edata.definitionPoint + data.definitionPoint)/2.0;
        double d = c.distanceTo(edata.definitionPoint);
        double a = c.angleTo(edata.definitionPoint + offset);
        
        RS_Vector v;
        v.setPolar(d, a);
        edata.definitionPoint = c + v;
        data.definitionPoint = c - v;
        update(true);
    }
    else if (ref.distanceTo(data.definitionPoint)<1.0e-4) {
        RS_Vector c = (edata.definitionPoint + data.definitionPoint)/2.0;
        double d = c.distanceTo(data.definitionPoint);
        double a = c.angleTo(data.definitionPoint + offset);
        
        RS_Vector v;
        v.setPolar(d, a);
        data.definitionPoint = c + v;
        edata.definitionPoint = c - v;
        update(true);
    }
    else if (ref.distanceTo(data.middleOfText)<1.0e-4) {
        data.middleOfText.move(offset);
        update(false);
    }
    else if (ref.distanceTo(autoTextPos)<1.0e-4) {
        data.middleOfText = autoTextPos + offset;
        update(false);
    }
}



RS_Variant RS_DimDiametric::getProperty(const RS_String& name, const RS_Variant& def) {
    if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT1_X) {
        return data.definitionPoint.x;
    } 
    else if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT1_Y) {
        return data.definitionPoint.y;
    } 
    else if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT2_X) {
        return edata.definitionPoint.x;
    } 
    else if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT2_Y) {
        return edata.definitionPoint.y;
    } 
    else if (name==RS_DIMDIAMETRIC_LEADER) {
        return edata.leader;
    } else {
        return RS_Dimension::getProperty(name, def);
    }
}





RS_StringList RS_DimDiametric::getPropertyNames(bool includeGeometry) {
    RS_StringList ret = RS_Dimension::getPropertyNames(includeGeometry);
    if (includeGeometry) {
        ret << RS_DIMDIAMETRIC_DEFINITIONPOINT1_X;
        ret << RS_DIMDIAMETRIC_DEFINITIONPOINT1_Y;
        ret << RS_DIMDIAMETRIC_DEFINITIONPOINT2_X;
        ret << RS_DIMDIAMETRIC_DEFINITIONPOINT2_Y;
        //ret << RS_DIMDIAMETRIC_LEADER;
    }
    // this entity renamed the definition point from RS_Dimension:
    ret.removeAll(RS_DIM_DEFINITIONPOINT_X);
    ret.removeAll(RS_DIM_DEFINITIONPOINT_Y);

    return ret;
}



void RS_DimDiametric::setProperty(const RS_String& name, const RS_Variant& value) {
    if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT1_X) {
        data.definitionPoint.x = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT1_Y) {
        data.definitionPoint.y = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT2_X) {
        edata.definitionPoint.x = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMDIAMETRIC_DEFINITIONPOINT2_Y) {
        edata.definitionPoint.y = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMDIAMETRIC_LEADER) {
        edata.leader = RS_Math::eval(value.toString());
        update(true);
    }
    
    RS_Dimension::setProperty(name, value);
}



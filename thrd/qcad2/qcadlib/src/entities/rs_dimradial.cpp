/****************************************************************************
** $Id: rs_dimradial.cpp 8945 2008-02-07 23:37:50Z andrew $
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


#include "rs_dimradial.h"
//#include "rs_constructionline.h"
#include "rs_text.h"
#include "rs_solid.h"
#include "rs_graphic.h"


const char* RS_DimRadial::propertyDefinitionPoint1X = QT_TRANSLATE_NOOP("QObject", "Point 1|X");
const char* RS_DimRadial::propertyDefinitionPoint1Y = QT_TRANSLATE_NOOP("QObject", "Point 1|Y");
const char* RS_DimRadial::propertyDefinitionPoint2X = QT_TRANSLATE_NOOP("QObject", "Point 2|X");
const char* RS_DimRadial::propertyDefinitionPoint2Y = QT_TRANSLATE_NOOP("QObject", "Point 2|Y");
const char* RS_DimRadial::propertyLeader = QT_TRANSLATE_NOOP("QObject", "Leader");


/**
 * Constructor.
 *
 * @para parentContainer Parent Entity Container.
 * @para d Common dimension geometrical data.
 * @para ed Extended geometrical data for radial dimension.
 */
RS_DimRadial::RS_DimRadial(RS_EntityContainer* parentContainer,
                           const RS_DimensionData& d,
                           const RS_DimRadialData& ed)
        : RS_Entity(parentContainer), RS_Dimension(parentContainer, d), edata(ed) {}



/**
 * @return Automatically created label for the default 
 * measurement of this dimension.
 */
RS_String RS_DimRadial::getMeasuredLabel() {

    // Definitive dimension line:
    double dist = data.definitionPoint.distanceTo(edata.definitionPoint);

    RS_Graphic* graphic = getGraphic();

    RS_String ret;
    if (graphic!=NULL) {
        ret = RS_Units::formatLinear(
            dist, graphic->getUnit(),
            graphic->getLinearFormat(), graphic->getLinearPrecision(),
            false,
            graphic->showLeadingZeroes(), graphic->showTrailingZeroes());
    } else {
        ret = RS_String("%1").arg(dist);
    }

    return ret;
}


RS_VectorList RS_DimRadial::getRefPoints(RS2::RefMode /*refMode*/) {
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
void RS_DimRadial::update(bool autoText) {

    RS_DEBUG->print("RS_DimRadial::update");

    clear();

    if (isUndone()) {
        return;
    }

    // dimension line:
    //updateCreateDimensionLine(data.definitionPoint, edata.definitionPoint,
    //false, true);

    RS_Vector p1 = data.definitionPoint;
    RS_Vector p2 = edata.definitionPoint;
    double angle = p1.angleTo(p2);

    // text height (DIMTXT)
    double dimtxt = getTextHeight();
    // text distance to line (DIMGAP)
    double dimgap = getDimensionLineGap();

    // length of dimension line:
    double length = p1.distanceTo(p2);

    RS_TextData textData;

    textData = RS_TextData(RS_Vector(0.0,0.0),
                           dimtxt, 30.0,
                           RS2::VAlignMiddle,
                           RS2::HAlignCenter,
                           RS2::LeftToRight,
                           RS2::Exact,
                           1.0,
                           getLabel(),
                           "standard",
                           0.0);

    RS_Text* text = new RS_Text(this, textData);
    double textWidth = text->getSize().x;

    // do we have to put the arrow / text outside of the arc?
    bool outsideArrow = (length<getArrowSize()*2+textWidth);
    double arrowAngle;

    if (outsideArrow) {
        length += getArrowSize()*2 + textWidth;
        arrowAngle = angle+M_PI;
    } else {
        arrowAngle = angle;
    }

    // create arrow:
    addEntity(createArrow(p2, arrowAngle));

    RS_Vector p3;
    p3.setPolar(length, angle);
    p3 += p1;

    // Create dimension line:
    RS_Line* dimensionLine = new RS_Line(this, RS_LineData(p1, p3));
    dimensionLine->setPen(RS_Pen(RS2::FlagInvalid));
    dimensionLine->setLayer(NULL);
    addEntity(dimensionLine);

    RS_Vector distV;
    double textAngle;

    // rotate text so it's readable from the bottom or right (ISO)
    // quadrant 1 & 4
    if (angle>M_PI/2.0*3.0+0.001 ||
            angle<M_PI/2.0+0.001) {

        distV.setPolar(dimgap + dimtxt/2.0, angle+M_PI/2.0);
        textAngle = angle;
    }
    // quadrant 2 & 3
    else {
        distV.setPolar(dimgap + dimtxt/2.0, angle-M_PI/2.0);
        textAngle = angle+M_PI;
    }

    // move text label:
    RS_Vector textPos;

    if (data.middleOfText.valid && !autoText) {
        textPos = data.middleOfText;
    } else {
        if (outsideArrow) {
            textPos.setPolar(length-textWidth/2.0-getArrowSize(), angle);
        } else {
            textPos.setPolar(length/2.0, angle);
        }
        textPos+=p1;
        // move text away from dimension line:
        textPos += distV;
        //data.middleOfText = textPos;
        autoTextPos = textPos;
    }

    text->rotate(RS_Vector(0.0,0.0), textAngle);
    text->move(textPos);

    text->setPen(RS_Pen(RS2::FlagInvalid));
    text->setLayer(NULL);
    addEntity(text);

    calculateBorders();
}



void RS_DimRadial::move(const RS_Vector& offset) {
    RS_Dimension::move(offset);

    edata.definitionPoint.move(offset);
    update();
}



void RS_DimRadial::rotate(const RS_Vector& center, double angle) {
    RS_Dimension::rotate(center, angle);

    edata.definitionPoint.rotate(center, angle);
    update();
}



void RS_DimRadial::scale(const RS_Vector& center, const RS_Vector& factor) {
    RS_Dimension::scale(center, factor);

    edata.definitionPoint.scale(center, factor);
    edata.leader*=factor.x;
    update();
}



void RS_DimRadial::mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2) {
    RS_Dimension::mirror(axisPoint1, axisPoint2);

    edata.definitionPoint.mirror(axisPoint1, axisPoint2);
    update();
}


void RS_DimRadial::moveRef(const RS_Vector& ref, const RS_Vector& offset) {

    if (ref.distanceTo(edata.definitionPoint)<1.0e-4) {
        double d = data.definitionPoint.distanceTo(edata.definitionPoint);
        double a = data.definitionPoint.angleTo(edata.definitionPoint + offset);
        
        RS_Vector v;
        v.setPolar(d, a);
        edata.definitionPoint = data.definitionPoint + v;
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


RS_Variant RS_DimRadial::getProperty(const RS_String& name, const RS_Variant& def) {
    if (name==RS_DIMRADIAL_DEFINITIONPOINT1_X) {
        return data.definitionPoint.x;
    } 
    else if (name==RS_DIMRADIAL_DEFINITIONPOINT1_Y) {
        return data.definitionPoint.y;
    } 
    else if (name==RS_DIMRADIAL_DEFINITIONPOINT2_X) {
        return edata.definitionPoint.x;
    } 
    else if (name==RS_DIMRADIAL_DEFINITIONPOINT2_Y) {
        return edata.definitionPoint.y;
    } 
    else if (name==RS_DIMRADIAL_LEADER) {
        return edata.leader;
    } else {
        return RS_Dimension::getProperty(name, def);
    }
}





RS_StringList RS_DimRadial::getPropertyNames(bool includeGeometry) {
    RS_StringList ret = RS_Dimension::getPropertyNames(includeGeometry);
    if (includeGeometry) {
        ret << RS_DIMRADIAL_DEFINITIONPOINT1_X;
        ret << RS_DIMRADIAL_DEFINITIONPOINT1_Y;
        ret << RS_DIMRADIAL_DEFINITIONPOINT2_X;
        ret << RS_DIMRADIAL_DEFINITIONPOINT2_Y;
        //ret << RS_DIMRADIAL_LEADER;
    }
    // this entity renamed the definition point from RS_Dimension:
    ret.removeAll(RS_DIM_DEFINITIONPOINT_X);
    ret.removeAll(RS_DIM_DEFINITIONPOINT_Y);

    return ret;
}



void RS_DimRadial::setProperty(const RS_String& name, const RS_Variant& value) {
    if (name==RS_DIMRADIAL_DEFINITIONPOINT1_X) {
        data.definitionPoint.x = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMRADIAL_DEFINITIONPOINT1_Y) {
        data.definitionPoint.y = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMRADIAL_DEFINITIONPOINT2_X) {
        edata.definitionPoint.x = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMRADIAL_DEFINITIONPOINT2_Y) {
        edata.definitionPoint.y = RS_Math::eval(value.toString());
        update(true);
    }
    if (name==RS_DIMRADIAL_LEADER) {
        edata.leader = RS_Math::eval(value.toString());
        update(true);
    }
    
    RS_Dimension::setProperty(name, value);
}



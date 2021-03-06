/****************************************************************************
** $Id: gv_dimangular.cpp 9428 2008-03-19 08:30:36Z andrew $
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
** not clear to you. Updated for Biluna 2016-08-11 Updated for Biluna 2016-08-11
**
**********************************************************************/


#include "gv_dimangular.h"

#include "gv_constructionline.h"
//#include "gv_filterdxf.h"
#include "gv_graphic.h"
#include "gv_information.h"
#include "gv_solid.h"
#include "gv_text.h"


const char* GV_DimAngular::propertyDefinitionPointX = QT_TRANSLATE_NOOP("QObject", "Leg 1|X1");
const char* GV_DimAngular::propertyDefinitionPointY = QT_TRANSLATE_NOOP("QObject", "Leg 1|Y1");
const char* GV_DimAngular::propertyDefinitionPoint1X = QT_TRANSLATE_NOOP("QObject", "Leg 2|X1");
const char* GV_DimAngular::propertyDefinitionPoint1Y = QT_TRANSLATE_NOOP("QObject", "Leg 2|Y1");
const char* GV_DimAngular::propertyDefinitionPoint2X = QT_TRANSLATE_NOOP("QObject", "Leg 2|X2");
const char* GV_DimAngular::propertyDefinitionPoint2Y = QT_TRANSLATE_NOOP("QObject", "Leg 2|Y2");
const char* GV_DimAngular::propertyDefinitionPoint3X = QT_TRANSLATE_NOOP("QObject", "Leg 1|X2");
const char* GV_DimAngular::propertyDefinitionPoint3Y = QT_TRANSLATE_NOOP("QObject", "Leg 1|Y2");
const char* GV_DimAngular::propertyDefinitionPoint4X = QT_TRANSLATE_NOOP("QObject", "Dim Pos|X");
const char* GV_DimAngular::propertyDefinitionPoint4Y = QT_TRANSLATE_NOOP("QObject", "Dim Pos|Y");


/**
 * Constructor.
 *
 * @para parentContainer Parent Entity Container.
 * @para d Common dimension geometrical data.
 * @para ed Extended geometrical data for angular dimension.
 */
GV_DimAngular::GV_DimAngular(GV_EntityContainer* parentContainer,
                             const GV_DimensionData& d,
                             const GV_DimAngularData& ed)
        : GV_Entity(parentContainer), GV_Dimension(parentContainer, d), edata(ed) {

    calculateBorders();
}



GV_VectorList GV_DimAngular::getRefPoints(GV2::RefMode /*refMode*/) {
    GV_VectorList ret;
    ret << edata.definitionPoint1;
    ret << edata.definitionPoint2;
    ret << edata.definitionPoint3;
    ret << edata.definitionPoint4;
    ret << data.definitionPoint;
    //ret << data.middleOfText;
    return ret;
}



/**
 * @return Automatically created label for the default 
 * measurement of this dimension.
 */
QString GV_DimAngular::getMeasuredLabel() {
    QString ret;

    GV2::AngleFormat dimaunit = GV2::DegreesDecimal;
    int dimadec = 0;

    GV_Graphic* graphic = getGraphic();
    if (graphic!=NULL) {
        dimaunit = graphic->getAngleFormat();
        dimadec = graphic->getAnglePrecision();
    }

    ret = GV_Units::formatAngle(getAngle(), 
        dimaunit,
        dimadec,
        graphic->showLeadingZeroesAngle(), 
        graphic->showTrailingZeroesAngle());
    
    return ret;
}



/**
 * @return Angle of the measured dimension.
 */
double GV_DimAngular::getAngle() {
    double ang1 = 0.0;
    double ang2 = 0.0;
    bool reversed = false;
    GV_Vector p1;
    GV_Vector p2;

    getAngles(ang1, ang2, reversed, p1, p2);

    if (!reversed) {
        if (ang2<ang1) {
            ang2+=2*M_PI;
        }
        return ang2-ang1;
    } else {
        if (ang1<ang2) {
            ang1+=2*M_PI;
        }
        return ang1-ang2;
    }
}



/**
 * @return Center of the measured dimension.
 */
GV_Vector GV_DimAngular::getCenter() {
    GV_ConstructionLine l1(NULL, GV_ConstructionLineData(edata.definitionPoint1,
                           edata.definitionPoint2));
    GV_ConstructionLine l2(NULL, GV_ConstructionLineData(edata.definitionPoint3,
                           data.definitionPoint));
    GV_VectorList vs = GV_Information::getIntersection(&l1, &l2, false);

    return vs.get(0);
}



/**
 * finds out which angles this dimension actually measures.
 *
 * @param ang1 Reference will return the start angle
 * @param ang2 Reference will return the end angle
 * @param reversed Reference will return the reversed flag.
 *
 * @return true: on success
 */
bool GV_DimAngular::getAngles(double& ang1, double& ang2, bool& reversed,
                              GV_Vector& p1, GV_Vector& p2) {

    GV_Vector center = getCenter();
    double ang = center.angleTo(edata.definitionPoint4);
    bool done = false;

    // find out the angles this dimension refers to:
    for (int f1=0; f1<=1 && !done; ++f1) {
        ang1 = GV_Math::correctAngle(
                   edata.definitionPoint1.angleTo(edata.definitionPoint2) + f1*M_PI);
        if (f1==0) {
            p1 = edata.definitionPoint2;
        } else {
            p1 = edata.definitionPoint1;
        }
        for (int f2=0; f2<=1 && !done; ++f2) {
            ang2 = GV_Math::correctAngle(
                       edata.definitionPoint3.angleTo(data.definitionPoint) + f2*M_PI);
            if (f2==0) {
                p2 = data.definitionPoint;
            } else {
                p2 = edata.definitionPoint3;
            }
            for (int t=0; t<=1 && !done; ++t) {
                reversed = (bool)t;

                double angDiff;

                if (!reversed) {
                    if (ang2<ang1) {
                        ang2+=2*M_PI;
                    }
                    angDiff = ang2-ang1;
                } else {
                    if (ang1<ang2) {
                        ang1+=2*M_PI;
                    }
                    angDiff = ang1-ang2;
                }

                ang1 = GV_Math::correctAngle(ang1);
                ang2 = GV_Math::correctAngle(ang2);

                if (GV_Math::isAngleBetween(ang, ang1, ang2, reversed) &&
                        angDiff<=M_PI) {
                    done = true;
                    break;
                }
            }
        }
    }

    return done;
}



/**
 * Updates the sub entities of this dimension. Called when the 
 * dimension or the position, alignment, .. changes.
 *
 * @param autoText Automatically reposition the text label
 */
void GV_DimAngular::update(bool /*autoText*/) {

    RB_DEBUG->print("GV_DimAngular::update");

    clear();
    
    if (isUndone()) {
        return;
    }

    // distance from entities (DIMEXO)
    double dimexo = getExtensionLineOffset();
    // extension line extension (DIMEXE)
    double dimexe = getExtensionLineExtension();
    // text height (DIMTXT)
    double dimtxt = getTextHeight();
    // text distance to line (DIMGAP)
    double dimgap = getDimensionLineGap();

    // find out center:
    GV_Vector center = getCenter();

    if (!center.valid) {
        return;
    }

    double ang1 = 0.0;
    double ang2 = 0.0;
    bool reversed = false;
    GV_Vector p1;
    GV_Vector p2;

    getAngles(ang1, ang2, reversed, p1, p2);

    double rad = edata.definitionPoint4.distanceTo(center);

    GV_Line* line;
    GV_Vector dir;
    double len;
    double dist;

    // 1st extension line:
    dist = center.distanceTo(p1);
    len = rad - dist + dimexe;
    dir.setPolar(1.0, ang1);
    line = new GV_Line(this,
                       GV_LineData(center + dir*dist + dir*dimexo,
                                   center + dir*dist + dir*len));
    line->setPen(GV_Pen(GV2::FlagInvalid));
    line->setLayer(NULL);
    addEntity(line);

    // 2nd extension line:
    dist = center.distanceTo(p2);
    len = rad - dist + dimexe;
    dir.setPolar(1.0, ang2);
    line = new GV_Line(this,
                       GV_LineData(center + dir*dist + dir*dimexo,
                                   center + dir*dist + dir*len));
    line->setPen(GV_Pen(GV2::FlagInvalid));
    line->setLayer(NULL);
    addEntity(line);

    // Create dimension line (arc):
    GV_Arc* arc = new GV_Arc(this,
                             GV_ArcData(center,
                                        rad, ang1, ang2, reversed));
    arc->setPen(GV_Pen(GV2::FlagInvalid));
    arc->setLayer(NULL);
    addEntity(arc);

    // length of dimension arc:
    double distance = arc->getLength();

    // do we have to put the arrows outside of the arc?
    bool outsideArrows = (distance<getArrowSize()*2);

    // arrow angles:
    double arrowAngle1, arrowAngle2;
    double arrowAng;
    if (rad>1.0e-6) {
        arrowAng = getArrowSize() / rad;
    }
    else {
        arrowAng = 0.0;
    }

    if (outsideArrows) {
        arrowAngle1 = arc->getDirection1();
        arrowAngle2 = arc->getDirection2();
    }
    else {
        GV_Vector v1, v2;
        if (!arc->isReversed()) {
            v1.setPolar(rad, arc->getAngle1()+arrowAng);
        } else {
            v1.setPolar(rad, arc->getAngle1()-arrowAng);
        }
        v1+=arc->getCenter();
        arrowAngle1 = arc->getStartpoint().angleTo(v1);


        if (!arc->isReversed()) {
            v2.setPolar(rad, arc->getAngle2()-arrowAng);
        } else {
            v2.setPolar(rad, arc->getAngle2()+arrowAng);
        }
        v2+=arc->getCenter();
        arrowAngle2 = arc->getEndpoint().angleTo(v2);
        
        arrowAngle1 = arrowAngle1+M_PI;
        arrowAngle2 = arrowAngle2+M_PI;
    }

    // Arrows:
    addEntity(createArrow(arc->getStartpoint(), arrowAngle1));
    addEntity(createArrow(arc->getEndpoint(), arrowAngle2));

    // text label:
    GV_TextData textData;
    GV_Vector textPos = arc->getMiddlepoint();

    GV_Vector distV;
    double textAngle;
    double dimAngle1 = textPos.angleTo(arc->getCenter())-M_PI/2.0;

    // rotate text so it's readable from the bottom or right (ISO)
    // quadrant 1 & 4
    if (dimAngle1>M_PI/2.0*3.0+0.001 ||
            dimAngle1<M_PI/2.0+0.001) {

        distV.setPolar(dimgap, dimAngle1+M_PI/2.0);
        textAngle = dimAngle1;
    }
    // quadrant 2 & 3
    else {
        distV.setPolar(dimgap, dimAngle1-M_PI/2.0);
        textAngle = dimAngle1+M_PI;
    }

    // move text away from dimension line:
    textPos+=distV;

    textData = GV_TextData(textPos,
                           dimtxt, 30.0,
                           GV2::VAlignBottom,
                           GV2::HAlignCenter,
                           GV2::LeftToRight,
                           GV2::Exact,
                           1.0,
                           getLabel(),
                           "standard",
                           textAngle);

    GV_Text* text = new GV_Text(this, textData);

    // move text to the side:
    text->setPen(GV_Pen(GV2::FlagInvalid));
    text->setLayer(NULL);
    addEntity(text);

    calculateBorders();
}



void GV_DimAngular::move(const GV_Vector& offset) {
    GV_Dimension::move(offset);

    edata.definitionPoint1.move(offset);
    edata.definitionPoint2.move(offset);
    edata.definitionPoint3.move(offset);
    edata.definitionPoint4.move(offset);
    update();
}



void GV_DimAngular::rotate(const GV_Vector& center, double angle) {
    GV_Dimension::rotate(center, angle);

    edata.definitionPoint1.rotate(center, angle);
    edata.definitionPoint2.rotate(center, angle);
    edata.definitionPoint3.rotate(center, angle);
    edata.definitionPoint4.rotate(center, angle);
    update();
}



void GV_DimAngular::scale(const GV_Vector& center, const GV_Vector& factor) {
    GV_Dimension::scale(center, factor);

    edata.definitionPoint1.scale(center, factor);
    edata.definitionPoint2.scale(center, factor);
    edata.definitionPoint3.scale(center, factor);
    edata.definitionPoint4.scale(center, factor);
    update();
}



void GV_DimAngular::mirror(const GV_Vector& axisPoint1, const GV_Vector& axisPoint2) {
    GV_Dimension::mirror(axisPoint1, axisPoint2);

    edata.definitionPoint1.mirror(axisPoint1, axisPoint2);
    edata.definitionPoint2.mirror(axisPoint1, axisPoint2);
    edata.definitionPoint3.mirror(axisPoint1, axisPoint2);
    edata.definitionPoint4.mirror(axisPoint1, axisPoint2);
    update();
}



void GV_DimAngular::moveRef(const GV_Vector& ref, const GV_Vector& offset) {
    if (ref.distanceTo(data.definitionPoint)<1.0e-4) {
        data.definitionPoint.move(offset);
        update(true);
    }
    else if (ref.distanceTo(edata.definitionPoint1)<1.0e-4) {
        edata.definitionPoint1.move(offset);
        update(true);
    }
    else if (ref.distanceTo(edata.definitionPoint2)<1.0e-4) {
        edata.definitionPoint2.move(offset);
        update(true);
    }
    else if (ref.distanceTo(edata.definitionPoint3)<1.0e-4) {
        edata.definitionPoint3.move(offset);
        update(true);
    }
    else if (ref.distanceTo(edata.definitionPoint4)<1.0e-4) {
        edata.definitionPoint4.move(offset);
        update(true);
    }
}



QVariant GV_DimAngular::getProperty(const QString& name, const QVariant& def) {
    if (name==GV_DIMANGULAR_DEFINITIONPOINT_X) {
        return data.definitionPoint.x;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT_Y) {
        return data.definitionPoint.y;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT1_X) {
        return edata.definitionPoint1.x;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT1_Y) {
        return edata.definitionPoint1.y;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT2_X) {
        return edata.definitionPoint2.x;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT2_Y) {
        return edata.definitionPoint2.y;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT3_X) {
        return edata.definitionPoint3.x;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT3_Y) {
        return edata.definitionPoint3.y;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT4_X) {
        return edata.definitionPoint4.x;
    } 
    else if (name==GV_DIMANGULAR_DEFINITIONPOINT4_Y) {
        return edata.definitionPoint4.y;
    } else {
        return GV_Dimension::getProperty(name, def);
    }
}



QStringList GV_DimAngular::getPropertyNames(bool includeGeometry) {
    QStringList ret = GV_Dimension::getPropertyNames(includeGeometry);
    if (includeGeometry) {
        ret << GV_DIMANGULAR_DEFINITIONPOINT4_X;
        ret << GV_DIMANGULAR_DEFINITIONPOINT4_Y;
        ret << GV_DIMANGULAR_DEFINITIONPOINT_X;
        ret << GV_DIMANGULAR_DEFINITIONPOINT_Y;
        ret << GV_DIMANGULAR_DEFINITIONPOINT1_X;
        ret << GV_DIMANGULAR_DEFINITIONPOINT1_Y;
        ret << GV_DIMANGULAR_DEFINITIONPOINT2_X;
        ret << GV_DIMANGULAR_DEFINITIONPOINT2_Y;
        ret << GV_DIMANGULAR_DEFINITIONPOINT3_X;
        ret << GV_DIMANGULAR_DEFINITIONPOINT3_Y;
    }
    // this entity renamed the definition point from GV_Dimension:
    ret.removeAll(GV_DIM_DEFINITIONPOINT_X);
    ret.removeAll(GV_DIM_DEFINITIONPOINT_Y);

    return ret;
}



void GV_DimAngular::setProperty(const QString& name, const QVariant& value) {
    if (name==GV_DIMANGULAR_DEFINITIONPOINT_X) {
        data.definitionPoint.x = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT_Y) {
        data.definitionPoint.y = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT1_X) {
        edata.definitionPoint1.x = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT1_Y) {
        edata.definitionPoint1.y = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT2_X) {
        edata.definitionPoint2.x = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT2_Y) {
        edata.definitionPoint2.y = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT3_X) {
        edata.definitionPoint3.x = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT3_Y) {
        edata.definitionPoint3.y = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT4_X) {
        edata.definitionPoint4.x = GV_Math::eval(value.toString());
        update(true);
    }
    if (name==GV_DIMANGULAR_DEFINITIONPOINT4_Y) {
        edata.definitionPoint4.y = GV_Math::eval(value.toString());
        update(true);
    }
    
    GV_Dimension::setProperty(name, value);
}



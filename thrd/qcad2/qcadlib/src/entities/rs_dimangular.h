/****************************************************************************
** $Id: rs_dimangular.h 7802 2008-01-03 10:30:18Z andrew $
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


#ifndef RS_DIMANGULAR_H
#define RS_DIMANGULAR_H

#include "rs_dimension.h"

#define RS_DIMANGULAR_DEFINITIONPOINT_X QObject::tr(RS_DimAngular::propertyDefinitionPointX)
#define RS_DIMANGULAR_DEFINITIONPOINT_Y QObject::tr(RS_DimAngular::propertyDefinitionPointY)
#define RS_DIMANGULAR_DEFINITIONPOINT1_X QObject::tr(RS_DimAngular::propertyDefinitionPoint1X)
#define RS_DIMANGULAR_DEFINITIONPOINT1_Y QObject::tr(RS_DimAngular::propertyDefinitionPoint1Y)
#define RS_DIMANGULAR_DEFINITIONPOINT2_X QObject::tr(RS_DimAngular::propertyDefinitionPoint2X)
#define RS_DIMANGULAR_DEFINITIONPOINT2_Y QObject::tr(RS_DimAngular::propertyDefinitionPoint2Y)
#define RS_DIMANGULAR_DEFINITIONPOINT3_X QObject::tr(RS_DimAngular::propertyDefinitionPoint3X)
#define RS_DIMANGULAR_DEFINITIONPOINT3_Y QObject::tr(RS_DimAngular::propertyDefinitionPoint3Y)
#define RS_DIMANGULAR_DEFINITIONPOINT4_X QObject::tr(RS_DimAngular::propertyDefinitionPoint4X)
#define RS_DIMANGULAR_DEFINITIONPOINT4_Y QObject::tr(RS_DimAngular::propertyDefinitionPoint4Y)

/**
 * Holds the data that defines a angular dimension entity.
 */
class CAD_EXPORT RS_DimAngularData {
public:
    /**
     * Default constructor. Leaves the data object uninitialized.
     */
    RS_DimAngularData() {}

    /**
     * Constructor with initialisation.
     *
     * @param definitionPoint Definition point of the angular dimension. 
     * @param leader Leader length.
     */
    RS_DimAngularData(const RS_Vector& definitionPoint1,
                      const RS_Vector& definitionPoint2,
                      const RS_Vector& definitionPoint3,
                      const RS_Vector& definitionPoint4) {
        this->definitionPoint1 = definitionPoint1;
        this->definitionPoint2 = definitionPoint2;
        this->definitionPoint3 = definitionPoint3;
        this->definitionPoint4 = definitionPoint4;
    }

public:
    /** Definition point 1. */
    RS_Vector definitionPoint1;
    /** Definition point 2. */
    RS_Vector definitionPoint2;
    /** Definition point 3. */
    RS_Vector definitionPoint3;
    /** Definition point 4. */
    RS_Vector definitionPoint4;
};



/**
 * Class for angular dimension entities.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_DimAngular : public RS_Dimension {
public:
    RS_DimAngular(RS_EntityContainer* parentContainer,
                 const RS_DimensionData& d,
                 const RS_DimAngularData& ed);
    virtual ~RS_DimAngular() {}

    virtual RS_Entity* clone() {
        RS_DimAngular* d = new RS_DimAngular(parentContainer, data, edata);
        d->copyPropertiesFrom(this);
        d->entities = entities;
        d->entities.setAutoDelete(entities.autoDelete());
        d->autoTextPos = autoTextPos;
        d->initId();
        d->detach();
        return d;
    }

    /** @return RS2::EntityDimAngular */
    virtual RS2::EntityType rtti() const {
        return RS2::EntityDimAngular;
    }
    virtual bool isOfType(RS2::EntityType t) const {
        return (t==RS2::EntityDimAngular || RS_Dimension::isOfType(t));
    }
    virtual RS_String typeName() const {
        return QObject::tr("Angular Dimension");
    }

    /**
     * @return Copy of data that defines the angular dimension. 
     * @see getData()
     */
    RS_DimAngularData getEData() const {
        return edata;
    }
    
    virtual RS_VectorList getRefPoints(RS2::RefMode refMode=RS2::RefAll);

    virtual RS_String getMeasuredLabel();
    double getAngle();
    RS_Vector getCenter();
    bool getAngles(double& ang1, double& ang2, bool& reversed,
        RS_Vector& p1, RS_Vector& p2);

    virtual void update(bool autoText=false);

    RS_Vector getDefinitionPoint1() {
        return edata.definitionPoint1;
    }
    RS_Vector getDefinitionPoint2() {
        return edata.definitionPoint2;
    }
    RS_Vector getDefinitionPoint3() {
        return edata.definitionPoint3;
    }
    RS_Vector getDefinitionPoint4() {
        return edata.definitionPoint4;
    }

    virtual void move(const RS_Vector& offset);
    virtual void rotate(const RS_Vector& center, double angle);
    virtual void scale(const RS_Vector& center, const RS_Vector& factor);
    virtual void mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2);
    virtual void moveRef(const RS_Vector& ref, const RS_Vector& offset);
    
    virtual RS_Variant getProperty(const RS_String& name, 
        const RS_Variant& def=RS_Variant());
    virtual RS_StringList getPropertyNames(bool includeGeometry=true);
    virtual void setProperty(const RS_String& name, const RS_Variant& value);

    static const char* propertyDefinitionPointX;
    static const char* propertyDefinitionPointY;
    static const char* propertyDefinitionPoint1X;
    static const char* propertyDefinitionPoint1Y;
    static const char* propertyDefinitionPoint2X;
    static const char* propertyDefinitionPoint2Y;
    static const char* propertyDefinitionPoint3X;
    static const char* propertyDefinitionPoint3Y;
    static const char* propertyDefinitionPoint4X;
    static const char* propertyDefinitionPoint4Y;

protected:
    /** Extended data. */
    RS_DimAngularData edata;
};

#endif

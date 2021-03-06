/****************************************************************************
** $Id: rs_dimdiametric.h 7802 2008-01-03 10:30:18Z andrew $
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


#ifndef RS_DIMDIAMETER_H
#define RS_DIMDIAMETER_H

#include "rs_dimension.h"

#define RS_DIMDIAMETRIC_DEFINITIONPOINT1_X QObject::tr(RS_DimDiametric::propertyDefinitionPoint1X)
#define RS_DIMDIAMETRIC_DEFINITIONPOINT1_Y QObject::tr(RS_DimDiametric::propertyDefinitionPoint1Y)
#define RS_DIMDIAMETRIC_DEFINITIONPOINT2_X QObject::tr(RS_DimDiametric::propertyDefinitionPoint2X)
#define RS_DIMDIAMETRIC_DEFINITIONPOINT2_Y QObject::tr(RS_DimDiametric::propertyDefinitionPoint2Y)
#define RS_DIMDIAMETRIC_LEADER QObject::tr(RS_DimDiametric::propertyLeader)

/**
 * Holds the data that defines a diametric dimension entity.
 */
class CAD_EXPORT RS_DimDiametricData {
public:
    /**
     * Default constructor. Leaves the data object uninitialized.
     */
    RS_DimDiametricData() {}

    /**
     * Constructor with initialisation.
     *
     * @param definitionPoint Definition point of the diametric dimension. 
     * @param leader Leader length.
     */
    RS_DimDiametricData(const RS_Vector& definitionPoint,
                     double leader) {
        this->definitionPoint = definitionPoint;
        this->leader = leader;
    }

public:
    /** Definition point. */
    RS_Vector definitionPoint;
    /** Leader length. */
    double leader;
};



/**
 * Class for diametric dimension entities.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_DimDiametric : public RS_Dimension {
public:
    RS_DimDiametric(RS_EntityContainer* parentContainer,
                 const RS_DimensionData& d,
                 const RS_DimDiametricData& ed);
    virtual ~RS_DimDiametric() {}

    virtual RS_Entity* clone() {
        RS_DimDiametric* d = new RS_DimDiametric(parentContainer, data, edata);
        d->copyPropertiesFrom(this);
        d->entities = entities;
        d->entities.setAutoDelete(entities.autoDelete());
        d->autoTextPos = autoTextPos;
        d->initId();
        d->detach();
        return d;
    }

    /**    @return RS2::EntityDimDiametric */
    virtual RS2::EntityType rtti() const {
        return RS2::EntityDimDiametric;
    }
    virtual bool isOfType(RS2::EntityType t) const {
        return (t==RS2::EntityDimDiametric || RS_Dimension::isOfType(t));
    }
    virtual RS_String typeName() const {
        return QObject::tr("Diametric Dimension");
    }

    /**
     * @return Copy of data that defines the diametric dimension. 
     * @see getData()
     */
    RS_DimDiametricData getEData() const {
        return edata;
    }
    
    virtual RS_VectorList getRefPoints(RS2::RefMode refMode=RS2::RefAll);

    virtual RS_String getMeasuredLabel();

    virtual void update(bool autoText=false);

    RS_Vector getDefinitionPoint() {
        return edata.definitionPoint;
    }
    double getLeader() {
        return edata.leader;
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

    static const char* propertyDefinitionPoint1X;
    static const char* propertyDefinitionPoint1Y;
    static const char* propertyDefinitionPoint2X;
    static const char* propertyDefinitionPoint2Y;
    static const char* propertyLeader;

protected:
    /** Extended data. */
    RS_DimDiametricData edata;
};

#endif

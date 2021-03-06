/****************************************************************************
** $Id: rs_dimaligned.h 7816 2008-01-04 16:57:10Z andrew $
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


#ifndef RS_DIMALIGNED_H
#define RS_DIMALIGNED_H

#include "rs_dimension.h"


#define RS_DIMALIGNED_EXTENSIONPOINT1_X QObject::tr(RS_DimAligned::propertyExtensionPoint1X)
#define RS_DIMALIGNED_EXTENSIONPOINT1_Y QObject::tr(RS_DimAligned::propertyExtensionPoint1Y)
#define RS_DIMALIGNED_EXTENSIONPOINT2_X QObject::tr(RS_DimAligned::propertyExtensionPoint2X)
#define RS_DIMALIGNED_EXTENSIONPOINT2_Y QObject::tr(RS_DimAligned::propertyExtensionPoint2Y)

/**
 * Holds the data that defines an aligned dimension entity.
 */
class CAD_EXPORT RS_DimAlignedData {
public:
    /**
     * Default constructor. Leaves the data object uninitialized.
     */
    RS_DimAlignedData() {}

    /**
     * Constructor with initialisation.
     *
     * @para extensionPoint1 Definition point. Startpoint of the 
     *         first extension line.
     * @para extensionPoint2 Definition point. Startpoint of the 
     *         second extension line.
     */
    RS_DimAlignedData(const RS_Vector& extensionPoint1,
                      const RS_Vector& extensionPoint2) {
        this->extensionPoint1 = extensionPoint1;
        this->extensionPoint2 = extensionPoint2;
    }

public:
    /** Definition point. Startpoint of the first extension line. */
    RS_Vector extensionPoint1;
    /** Definition point. Startpoint of the second extension line. */
    RS_Vector extensionPoint2;
};



/**
 * Class for aligned dimension entities.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_DimAligned : public RS_Dimension {
public:
    RS_DimAligned(RS_EntityContainer* parentContainer,
                  const RS_DimensionData& d,
                  const RS_DimAlignedData& ed);
    virtual ~RS_DimAligned() {}

    virtual RS_Entity* clone() {
        RS_DimAligned* d = new RS_DimAligned(parentContainer, data, edata);
        d->copyPropertiesFrom(this);
        d->entities = entities;
        d->entities.setAutoDelete(entities.autoDelete());
        d->autoTextPos = autoTextPos;
        d->initId();
        d->detach();
        d->refDefinitionPoint1 = refDefinitionPoint1;
        d->refDefinitionPoint2 = refDefinitionPoint2;
        return d;
    }

    /** @return RS2::EntityDimAligned */
    virtual RS2::EntityType rtti() const {
        return RS2::EntityDimAligned;
    }
    virtual bool isOfType(RS2::EntityType t) const {
        return (t==RS2::EntityDimAligned || RS_Dimension::isOfType(t));
    }
    virtual RS_String typeName() const {
        return QObject::tr("Aligned Dimension");
    }

    /**
     * @return Copy of data that defines the aligned dimension. 
     * @see getData()
     */
    RS_DimAlignedData getEData() const {
        return edata;
    }

    virtual RS_VectorList getRefPoints(RS2::RefMode refMode=RS2::RefAll);

    virtual RS_String getMeasuredLabel();

    virtual void update(bool autoText=false);

    RS_Vector getExtensionPoint1() {
        return edata.extensionPoint1;
    }

    RS_Vector getExtensionPoint2() {
        return edata.extensionPoint2;
    }
    
    virtual bool hasEndpointsWithinWindow(const RS_Vector& v1, const RS_Vector& v2);

    virtual void move(const RS_Vector& offset);
    virtual void rotate(const RS_Vector& center, double angle);
    virtual void scale(const RS_Vector& center, const RS_Vector& factor);
    virtual void mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2);
    virtual void stretch(const RS_Vector& firstCorner,
                         const RS_Vector& secondCorner,
                         const RS_Vector& offset);
    virtual void moveRef(const RS_Vector& ref, const RS_Vector& offset);
    
    virtual RS_Variant getProperty(const RS_String& name, 
        const RS_Variant& def=RS_Variant());
    virtual RS_StringList getPropertyNames(bool includeGeometry=true);
    virtual void setProperty(const RS_String& name, const RS_Variant& value);

    static const char* propertyExtensionPoint1X;
    static const char* propertyExtensionPoint1Y;
    static const char* propertyExtensionPoint2X;
    static const char* propertyExtensionPoint2Y;

protected:
    /** Extended data. */
    RS_DimAlignedData edata;
    RS_Vector refDefinitionPoint1;
    RS_Vector refDefinitionPoint2;
};

#endif

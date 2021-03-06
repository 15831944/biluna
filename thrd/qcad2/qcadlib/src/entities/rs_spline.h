/****************************************************************************
** $Id: rs_spline.h 9362 2008-03-08 14:34:04Z andrew $
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


#ifndef RS_SPLINE_H
#define RS_SPLINE_H

#include "rs_entitycontainer.h"
#include "rs_valuelist.h"

#define RS_SPLINE_DEGREE   QObject::tr(RS_Spline::degree)
#define RS_SPLINE_CLOSED   QObject::tr(RS_Spline::closed)

/**
 * Holds the data that defines a line.
 */
class CAD_EXPORT RS_SplineData {
public:
    /**
     * Default constructor. Leaves the data object uninitialized.
     */
    RS_SplineData() {}
    
    RS_SplineData(int degree, bool closed) {
        this->degree = degree;
        this->closed = closed;
    }

public:
    /** Degree of the spline (1, 2, 3) */
    int degree;
    /** Closed flag. */
    bool closed;
    /** Control points of the spline. */
    RS_ValueList<RS_Vector> controlPoints;
};


/**
 * Class for a spline entity.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_Spline : public RS_EntityContainer {
public:
    RS_Spline(RS_EntityContainer* parentContainer,
            const RS_SplineData& d);

    virtual RS_Entity* clone();

    virtual ~RS_Spline();

    /**    @return RS2::EntitySpline */
    virtual RS2::EntityType rtti() const {
        return RS2::EntitySpline;
    }
    virtual bool isOfType(RS2::EntityType t) const {
        return (t==RS2::EntitySpline || RS_EntityContainer::isOfType(t));
    }
    virtual RS_String typeName() const {
        return QObject::tr("Spline");
    }
    /** @return false */
    virtual bool isEdge() const {
        return false;
    }

    /** @return Copy of data that defines the spline. */
    RS_SplineData getData() const {
        return data;
    }
    
    /** Sets the splines degree (1-3). */
    void setDegree(int deg) {
        if (deg>=1 && deg<=3) {
            data.degree = deg;
        }
    }

    /** @return Degree of this spline curve (1-3).*/
    int getDegree() {
        return data.degree;
    }

    /** @return 0. */
    int getNumberOfKnots() {
        return 0;
    }

    /** @return Number of control points. */
    int getNumberOfControlPoints() {
        return data.controlPoints.count();
    }

    /** 
     * @retval true if the spline is closed.
     * @retval false otherwise.
     */
    bool isClosed() {
        return data.closed;
    }

    /**
     * Sets the closed falg of this spline.
     */
    void setClosed(bool c) {
        data.closed = c;
        update();
    }

    virtual RS_VectorList getRefPoints(RS2::RefMode refMode=RS2::RefAll);
    virtual RS_Vector getNearestRef(const RS_Vector& coord,
                                     double* dist = NULL,
                                     RS2::RefMode refMode=RS2::RefAll);
    virtual RS_Vector getNearestSelectedRef(const RS_Vector& coord,
                                     double* dist = NULL,
                                     RS2::RefMode refMode=RS2::RefAll);


    void update();
    
    virtual RS_Vector getNearestEndpoint(const RS_Vector& coord,
                                         double* dist = NULL);
    virtual RS_Vector getNearestCenter(const RS_Vector& coord,
                                       double* dist = NULL);
    virtual RS_Vector getNearestMiddle(const RS_Vector& coord,
                                       double* dist = NULL);
    virtual RS_Vector getNearestDist(double distance,
                                     const RS_Vector& coord,
                                     double* dist = NULL);
    
    virtual void addControlPoint(const RS_Vector& v);
    virtual void removeLastControlPoint();

    virtual void move(const RS_Vector& offset);
    virtual void rotate(const RS_Vector& center, double angle);
    virtual void scale(const RS_Vector& center, const RS_Vector& factor);
    virtual void mirror(const RS_Vector& axisPoint1, 
        const RS_Vector& axisPoint2);
    virtual void moveRef(const RS_Vector& ref, const RS_Vector& offset);

    virtual void draw(RS_Painter* painter, RS_GraphicView* view, 
        double patternOffset=0.0);
    RS_ValueList<RS_Vector> getControlPoints();

    static void generateBasisFunctions(int c, double t, int npts, 
        int x[], double h[], double r[]);
    
    static void knotOpen(int num, int order, int knotVector[]);
    static void calculatePointOpen(int npts, int k, int p1,
        double b[], double h[], double p[]);
        
    static void knotClosed(int num, int order, int knotVector[]);
    static void calculatePointsClosed(int npts, int k, int p1,
        double b[], double h[], double p[]);

    virtual RS_Variant getProperty(const RS_String& name, 
        const RS_Variant& def=RS_Variant());
    virtual RS_StringList getPropertyNames(bool includeGeometry=true);
    virtual void setProperty(const RS_String& name, const RS_Variant& value);
    
    virtual RS_PropertyAttributes getPropertyAttributes(
        const RS_String& propertyName);

protected:
    RS_SplineData data;
    
    static const char* degree;
    static const char* closed;
};

#endif

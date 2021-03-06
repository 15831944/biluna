/****************************************************************************
** $Id: rs_vector.h 7519 2007-12-07 10:26:06Z andrew $
**
** Copyright (C) 2001-2007 RibbonSoft. All rights reserved.
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


#ifndef RS_VECTOR_H
#define RS_VECTOR_H

#include <iostream>

#include "rs.h"
#include "rs_list.h"

class RS_Matrix;


/**
 * Represents a 3d vector (x/y/z)
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_Vector {
public:
    inline RS_Vector();
    inline RS_Vector(double vx, double vy, double vz=0.0, bool valid=true);
    inline explicit RS_Vector(bool valid_in);
    //~RS_Vector();

    inline void set(double vx, double vy, double vz=0.0) {
        x = vx;
        y = vy;
        z = vz;
        valid = true;
    }
    void setPolar(double radius, double angle);

    bool isValid() const;

    double distanceTo(const RS_Vector& v) const;
    double distanceTo2d(const RS_Vector& v) const;
    double angle() const;
    double angleToPlaneXY() const;
    double angleTo(const RS_Vector& v) const;
    double magnitude() const;
    double magnitude2d() const;
    RS_Vector lerp(const RS_Vector& v, double t) const;

    bool isInWindow(const RS_Vector& firstCorner, const RS_Vector& secondCorner);

    RS_Vector move(const RS_Vector& offset);
    RS_Vector rotate(double ang);
    RS_Vector rotate(const RS_Vector& center, double ang);
    RS_Vector scale(double factor);
    RS_Vector scale(const RS_Vector& factor);
    RS_Vector scale(const RS_Vector& center, double factor);
    RS_Vector scale(const RS_Vector& center, const RS_Vector& factor);
    RS_Vector mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2);
    RS_Vector isoProject(RS2::IsoProjectionType type);
    RS_Vector transform(const RS_Matrix& m);

    RS_Vector operator + (const RS_Vector& v) const;
    RS_Vector operator - (const RS_Vector& v) const;
    RS_Vector operator * (double s) const;
    RS_Vector operator / (double s) const;
    RS_Vector operator - () const;

    void operator += (const RS_Vector& v);
    void operator -= (const RS_Vector& v);
    void operator *= (double s);

    bool operator == (const RS_Vector& v) const;
    bool operator != (const RS_Vector& v) const {
        return !operator==(v);
    }

    static RS_Vector minimum(const RS_Vector& v1, const RS_Vector& v2);
    static RS_Vector maximum(const RS_Vector& v1, const RS_Vector& v2);
    static RS_Vector crossP(const RS_Vector& v1, const RS_Vector& v2);
    static double dotP(const RS_Vector& v1, const RS_Vector& v2);
    static RS_Vector polar(double radius, double angle) {
        RS_Vector ret;
        ret.setPolar(radius, angle);
        return ret;
    }

public:
    double x;
    double y;
    double z;
    bool valid;
};


#ifdef RS_COMP
#define RS_VectorSolutions RS_VectorList
#endif

/**
 * Represents a list of vectors. Typically used to return multiple
 * solutions from a function. Previously known as RS_VectorSolutions.
 */
class /* CAD_EXPORT did not work */ RS_VectorList : public RS_List<RS_Vector> {
public:
    RS_VectorList();
    //RS_VectorList(const RS_VectorList& s);
#ifdef RS_COMP
    RS_VectorList(int num);
#endif
    RS_VectorList(const RS_Vector& v1);
    RS_VectorList(const RS_Vector& v1, const RS_Vector& v2);
    RS_VectorList(const RS_Vector& v1, const RS_Vector& v2,
                       const RS_Vector& v3);
    RS_VectorList(const RS_Vector& v1, const RS_Vector& v2,
                       const RS_Vector& v3, const RS_Vector& v4);
    RS_VectorList(const RS_Vector& v1, const RS_Vector& v2,
                       const RS_Vector& v3, const RS_Vector& v4,
                       const RS_Vector& v5);
    
    ~RS_VectorList();

#ifdef RS_COMP
    void alloc(int num);
    void clean();
#endif
    void clear();
    RS_Vector get(int i) const;
#ifdef RS_COMP
    int getNumber() const;
#endif
    bool hasValid() const;
    void set(int i, const RS_Vector& v);
    void setTangent(bool t);
    bool isTangent() const;
    RS_Vector getClosest(const RS_Vector& coord, 
            double* dist=NULL, int* index=NULL) const;
    void move(const RS_Vector& offset);
    void rotate(const RS_Vector& center, double ang);
    void scale(const RS_Vector& center, const RS_Vector& factor);

    void sortByDistanceTo(const RS_Vector& v, bool unique=false);

    RS_VectorList operator + (const RS_VectorList& s1);

private:
    //RS_Vector* vector;
    bool tangent;
};


/**
 * Constructor for a point with default coordinates.
 */
inline RS_Vector::RS_Vector() :
   x(0.0), y(0.0), z(0.0), valid(true)
{}

/**
 * Constructor for a point with given valid flag.
 *
 * @param valid true: a valid vector with default coordinates is created.
 *              false: an invalid vector is created
 */
inline RS_Vector::RS_Vector(bool valid_in) :
   x(0.0), y(0.0), z(0.0), valid(valid_in)
{}

/**
 * Constructor for a point with given coordinates.
 */
inline RS_Vector::RS_Vector(double vx, double vy, double vz, bool valid_in)  :
   x(vx), y(vy), z(vz), valid(valid_in)
{}

#endif

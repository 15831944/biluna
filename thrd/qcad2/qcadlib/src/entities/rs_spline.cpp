/****************************************************************************
** $Id: rs_spline.cpp 9379 2008-03-10 20:47:45Z andrew $
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


#include "rs_spline.h"

#include "rs_application.h"
#include "rs_debug.h"
#include "rs_graphicview.h"
#include "rs_painter.h"
#include "rs_graphic.h"


const char* RS_Spline::degree = QT_TRANSLATE_NOOP("QObject", "Degree");
const char* RS_Spline::closed = QT_TRANSLATE_NOOP("QObject", "Closed");


/**
 * Constructor.
 */
RS_Spline::RS_Spline(RS_EntityContainer* parentContainer,
                     const RS_SplineData& d)
        : RS_Entity(parentContainer), RS_EntityContainer(parentContainer), data(d) {
    calculateBorders();
}



/**
 * Destructor.
 */
RS_Spline::~RS_Spline() {}




RS_Entity* RS_Spline::clone() {
    RS_Spline* l = new RS_Spline(parentContainer, data);
    l->copyPropertiesFrom(this);
    l->entities = entities;
    l->entities.setAutoDelete(entities.autoDelete());
    l->initId();
    l->detach();
    return l;
}



RS_VectorList RS_Spline::getRefPoints(RS2::RefMode /*refMode*/) {

    RS_VectorList ret;

    int i=0;
    RS_ValueList<RS_Vector>::iterator it;
    for (it = data.controlPoints.begin();
            it!=data.controlPoints.end(); ++it, ++i) {

        ret.set(i, (*it));
    }

    return ret;
}



RS_Vector RS_Spline::getNearestRef(const RS_Vector& coord,
                                   double* dist,
                                   RS2::RefMode refMode) {

    //return getRefPoints().getClosest(coord, dist);
    return RS_Entity::getNearestRef(coord, dist, refMode);
}



RS_Vector RS_Spline::getNearestSelectedRef(const RS_Vector& coord,
        double* dist,
        RS2::RefMode refMode) {

    //return getRefPoints().getClosest(coord, dist);
    return RS_Entity::getNearestSelectedRef(coord, dist, refMode);
}


/**
 * Updates the internal polygon of this spline. Called when the 
 * spline or it's data, position, .. changes.
 */
void RS_Spline::update() {

    RS_DEBUG->print("RS_Spline::update");

    clear();

    if (isUndone()) {
        return;
    }

    if (data.degree<1 || data.degree>3) {
        RS_DEBUG->print("RS_Spline::update: invalid degree: %d", data.degree);
        return;
    }

    if (data.controlPoints.count()<data.degree+1) {
        RS_DEBUG->print("RS_Spline::update: not enough control points");
        return;
    }

    resetBorders();

    RS_ValueList<RS_Vector> tControlPoints = data.controlPoints;

    if (data.closed) {
        for (int i=0; i<data.degree; ++i) {
            tControlPoints.append(data.controlPoints[i]);
        }
    }

    int i;
    int npts = tControlPoints.count();
    // order:
    int k = data.degree+1;
    // resolution:
    int p1 = getGraphicVariableInt("$SPLINESEGS", 8) * npts;

    double* b = new double[npts*3+1];
    double* h = new double[npts+1];
    double* p = new double[p1*3+1];

    RS_ValueList<RS_Vector>::iterator it;
    i = 1;
    for (it = tControlPoints.begin(); it!=tControlPoints.end(); ++it) {
        b[i] = (*it).x;
        b[i+1] = (*it).y;
        b[i+2] = 0.0;

        RS_DEBUG->print("RS_Spline::update: b[%d]: %f/%f", i, b[i], b[i+1]);
        i+=3;
    }

    // set all homogeneous weighting factors to 1.0
    for (i=1; i <= npts; i++) {
        h[i] = 1.0;
    }

    for (i = 1; i <= 3*p1; i++) {
        p[i] = 0.0;
    }

    if (data.closed) {
        calculatePointsClosed(npts,k,p1,b,h,p);
    } else {
        calculatePointOpen(npts,k,p1,b,h,p);
    }

    RS_Vector prev(false);
    for (i = 1; i <= 3*p1; i=i+3) {
        if (prev.valid) {
            RS_Line* line = new RS_Line(this,
                                        RS_LineData(prev, RS_Vector(p[i], p[i+1])));
            line->setLayer(NULL);
            line->setPen(RS_Pen(RS2::FlagInvalid));
            if (isSelected()) {
                line->setSelected(true);
            }
            addEntity(line);
        }
        prev = RS_Vector(p[i], p[i+1]);

        minV = RS_Vector::minimum(prev, minV);
        maxV = RS_Vector::maximum(prev, maxV);
    }

    delete[] b;
    delete[] h;
    delete[] p;

    calculateBorders();
}



RS_Vector RS_Spline::getNearestEndpoint(const RS_Vector& coord,
                                        double* dist) {
    double minDist = RS_MAXDOUBLE;
    RS_Vector ret(false);

    if (!isClosed()) {
        RS_Vector startPoint = data.controlPoints[0];
        RS_Vector endPoint = data.controlPoints[data.controlPoints.count()-1];

        double d1 = startPoint.distanceTo(coord);
        double d2 = endPoint.distanceTo(coord);

        if (d1 < d2) {
            ret = startPoint;
            minDist = d1;
        }
        else {
            ret = endPoint;
            minDist = d2;
        }
    }

    if (dist!=NULL) {
        *dist = minDist;
    }

    return ret;
}



RS_Vector RS_Spline::getNearestCenter(const RS_Vector& /*coord*/,
                                      double* dist) {

    if (dist!=NULL) {
        *dist = RS_MAXDOUBLE;
    }

    return RS_Vector(false);
}



RS_Vector RS_Spline::getNearestMiddle(const RS_Vector& /*coord*/,
                                      double* dist) {
    if (dist!=NULL) {
        *dist = RS_MAXDOUBLE;
    }

    return RS_Vector(false);
}



RS_Vector RS_Spline::getNearestDist(double /*distance*/,
                                    const RS_Vector& /*coord*/,
                                    double* dist) {
    if (dist!=NULL) {
        *dist = RS_MAXDOUBLE;
    }

    return RS_Vector(false);
}



void RS_Spline::move(const RS_Vector& offset) {
    RS_ValueList<RS_Vector>::iterator it;
    for (it = data.controlPoints.begin();
            it!=data.controlPoints.end(); ++it) {

        (*it).move(offset);
    }

    update();
}



void RS_Spline::rotate(const RS_Vector& center, double angle) {
    RS_ValueList<RS_Vector>::iterator it;
    for (it = data.controlPoints.begin();
            it!=data.controlPoints.end(); ++it) {

        (*it).rotate(center, angle);
    }

    update();
}



void RS_Spline::scale(const RS_Vector& center, const RS_Vector& factor) {
    RS_ValueList<RS_Vector>::iterator it;
    for (it = data.controlPoints.begin();
            it!=data.controlPoints.end(); ++it) {

        (*it).scale(center, factor);
    }

    update();
}



void RS_Spline::mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2) {
    RS_ValueList<RS_Vector>::iterator it;
    for (it = data.controlPoints.begin();
            it!=data.controlPoints.end(); ++it) {

        (*it).mirror(axisPoint1, axisPoint2);
    }

    update();
}



void RS_Spline::moveRef(const RS_Vector& ref, const RS_Vector& offset) {
    RS_ValueList<RS_Vector>::iterator it;
    for (it = data.controlPoints.begin();
            it!=data.controlPoints.end(); ++it) {

        if (ref.distanceTo(*it)<1.0e-4) {
            (*it).move(offset);
        }
    }

    update();
}




void RS_Spline::draw(RS_Painter* painter, RS_GraphicView* view,
                     double /*patternOffset*/) {

    if (painter==NULL || view==NULL) {
        return;
    }

    RS_Entity* e = firstEntity(RS2::ResolveNone);
    double offset = 0.0;

    if (e!=NULL) {
        view->drawEntity(e);
        offset+=e->getLength();
    }

    for (RS_Entity* e=nextEntity(RS2::ResolveNone);
            e!=NULL;
            e = nextEntity(RS2::ResolveNone)) {

        view->drawEntityPlain(e, -offset);
        offset+=e->getLength();
    }
}



/**
 * @return The reference points of the spline.
 */
RS_ValueList<RS_Vector> RS_Spline::getControlPoints() {
    return data.controlPoints;
}



/**
 * Appends the given point to the control points.
 */
void RS_Spline::addControlPoint(const RS_Vector& v) {
    data.controlPoints.append(v);
}



/**
 * Removes the control point that was last added.
 */
void RS_Spline::removeLastControlPoint() {
    data.controlPoints.pop_back();
}


/**
 * Generates B-Spline open knot vector with multiplicity 
 * equal to the order at the ends.
 */
void RS_Spline::knotOpen(int num, int order, int knotVector[]) {
    knotVector[1] = 0;
    for (int i = 2; i <= num + order; i++) {
        if ( (i > order) && (i < num + 2) ) {
            knotVector[i] = knotVector[i-1] + 1;
        } else {
            knotVector[i] = knotVector[i-1];
        }
    }
}



/**
 * Generates rational B-spline basis functions for an open knot vector.
 */
void RS_Spline::generateBasisFunctions(int c, double t, int npts,
                       int x[], double h[], double r[]) {

    int nplusc;
    int i,k;
    double d,e;
    double sum;
    //double temp[36];

    nplusc = npts + c;

    double* temp = new double[nplusc+1];

    // calculate the first order nonrational basis functions n[i]
    for (i = 1; i<= nplusc-1; i++) {
        if (( t >= x[i]) && (t < x[i+1]))
            temp[i] = 1;
        else
            temp[i] = 0;
    }

    /* calculate the higher order nonrational basis functions */

    for (k = 2; k <= c; k++) {
        for (i = 1; i <= nplusc-k; i++) {
            // if the lower order basis function is zero skip the calculation
            if (temp[i] != 0)
                d = ((t-x[i])*temp[i])/(x[i+k-1]-x[i]);
            else
                d = 0;
            // if the lower order basis function is zero skip the calculation
            if (temp[i+1] != 0)
                e = ((x[i+k]-t)*temp[i+1])/(x[i+k]-x[i+1]);
            else
                e = 0;

            temp[i] = d + e;
        }
    }

    // pick up last point
    if (t == (double)x[nplusc]) {
        temp[npts] = 1;
    }

    // calculate sum for denominator of rational basis functions
    sum = 0.;
    for (i = 1; i <= npts; i++) {
        sum = sum + temp[i]*h[i];
    }

    // form rational basis functions and put in r vector
    for (i = 1; i <= npts; i++) {
        if (sum != 0) {
            r[i] = (temp[i]*h[i])/(sum);
        } else
            r[i] = 0;
    }

    delete[] temp;
}


/**
 * generates a rational B-spline curve using a uniform open knot vector.
 */
void RS_Spline::calculatePointOpen(int npts, int k, int p1,
                         double b[], double h[], double p[]) {

    int i,j,icount,jcount;
    int i1;
    int nplusc;

    double step;
    double t;
    double temp;

    nplusc = npts + k;

    int* x = new int[nplusc+1];
    double* nbasis = new double[npts+1];

    // zero and redimension the knot vector and the basis array
    for(i = 0; i <= npts; i++) {
        nbasis[i] = 0.0;
    }

    for(i = 0; i <= nplusc; i++) {
        x[i] = 0;
    }

    // generate the uniform open knot vector
    knotOpen(npts,k,x);

    icount = 0;

    // calculate the points on the rational B-spline curve
    t = 0;
    step = ((double)x[nplusc])/((double)(p1-1));

    for (i1 = 1; i1<= p1; i1++) {

        if ((double)x[nplusc] - t < 5e-6) {
            t = (double)x[nplusc];
        }

        // generate the basis function for this value of t
        generateBasisFunctions(k,t,npts,x,h,nbasis);

        // generate a point on the curve
        for (j = 1; j <= 3; j++) {
            jcount = j;
            p[icount+j] = 0.;

            // Do local matrix multiplication
            for (i = 1; i <= npts; i++) {
                temp = nbasis[i]*b[jcount];
                p[icount + j] = p[icount + j] + temp;
                jcount = jcount + 3;
            }
        }
        icount = icount + 3;
        t = t + step;
    }

    delete[] x;
    delete[] nbasis;
}


void RS_Spline::knotClosed(int num, int order, int knotVector[]) {
    int nplusc,nplus2,i;

    nplusc = num + order;
    nplus2 = num + 2;

    knotVector[1] = 0;
    for (i = 2; i <= nplusc; i++) {
        knotVector[i] = i-1;
    }
}



/**
 * generates a rational B-spline curve using a uniform closed knot vector.
 */
void RS_Spline::calculatePointsClosed(int npts, int k, int p1,
                         double b[], double h[], double p[]) {

    int i,j,icount,jcount;
    int i1;
    int nplusc;

    double step;
    double t;
    double temp;


    nplusc = npts + k;

    int* x = new int[nplusc+1];
    double* nbasis = new double[npts+1];

    // redimension the knot vector and the basis array
    for(i = 0; i <= npts; i++) {
        nbasis[i] = 0.0;
    }

    for(i = 0; i <= nplusc; i++) {
        x[i] = 0;
    }

    // generate the uniform periodic knot vector
    knotClosed(npts,k,x);
    icount = 0;

    // calculate the points on the rational B-spline curve
    t = k-1;
    step = ((double)((npts)-(k-1)))/((double)(p1-1));

    for (i1 = 1; i1<= p1; i1++) {

        if ((double)x[nplusc] - t < 5e-6) {
            t = (double)x[nplusc];
        }

        // generate the basis function for this value of t
        generateBasisFunctions(k,t,npts,x,h,nbasis);
        // generate a point on the curve:
        for (j = 1; j <= 3; j++) {
            jcount = j;
            p[icount+j] = 0.;

            // local matrix multiplication
            for (i = 1; i <= npts; i++) {
                temp = nbasis[i]*b[jcount];
                p[icount + j] = p[icount + j] + temp;
                jcount = jcount + 3;
            }
        }
        icount = icount + 3;
        t = t + step;
    }

    delete[] x;
    delete[] nbasis;
}


RS_Variant RS_Spline::getProperty(const RS_String& name, const RS_Variant& def) {
    if (name==RS_SPLINE_DEGREE) {
        return getDegree();
    } else if (name==RS_SPLINE_CLOSED) {
        return isClosed();
    } else {
        return RS_EntityContainer::getProperty(name, def);
    }
}



RS_StringList RS_Spline::getPropertyNames(bool includeGeometry) {
    RS_StringList ret = RS_EntityContainer::getPropertyNames(includeGeometry);
    if (includeGeometry) {
        ret << RS_SPLINE_DEGREE;
        ret << RS_SPLINE_CLOSED;
    }
    return ret;
}


void RS_Spline::setProperty(const RS_String& name, const RS_Variant& value) {
    if (name==RS_SPLINE_DEGREE) {
        setDegree(value.toInt());
    } else if (name==RS_SPLINE_CLOSED) {
        setClosed(variantToBool(value));
    } else {
        RS_EntityContainer::setProperty(name, value);
    }
    update();
}


/**
 * Limits the choices for the spline degree property.
 */
RS_PropertyAttributes RS_Spline::getPropertyAttributes(const RS_String& propertyName) {
    RS_PropertyAttributes ret = 
        RS_EntityContainer::getPropertyAttributes(propertyName);

    if (propertyName==RS_SPLINE_DEGREE) {
        ret.choices << "2";
        ret.choices << "3";
    }

    return ret;
}

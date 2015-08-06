#include <cassert>
#include <cmath>

#include "RBox.h"
#include "RDebug.h"
#include "RLine.h"
#include "RMath.h"
#include "RTriangle.h"

/**
 * Creates a triangle with invalid corner points.
 */
RTriangle::RTriangle() {
}

/**
 * Creates a triangle with the given corners.
 */
RTriangle::RTriangle(const RVector& p1, const RVector& p2, const RVector& p3) {

	corner[0] = p1;
	corner[1] = p2;
	corner[2] = p3;
}

RTriangle::~RTriangle() {
}

RBox RTriangle::getBoundingBox() const {
	return RBox(RVector::getMinimum(RVector::getMinimum(corner[0], corner[1]),
			corner[2]), RVector::getMaximum(RVector::getMaximum(corner[0],
			corner[1]), corner[2]));
}

double RTriangle::getDistanceTo(const RVector& point, bool limited) const {

	RVector normal = getNormal();
	double d = getD();
	double distance = (normal.x * point.x + normal.y * point.y + normal.z
			* point.z + d) / (normal.getMagnitude());

	if (!limited
			|| isPointInTriangle(point - normal.getUnitVector() * distance)) {
		return distance;
	}

	return RMAXDOUBLE;
}

RVector RTriangle::getVectorTo(const RVector& /*point*/, bool /*limited*/) const {
	assert(false);
	return RVector();
}

/*
RVector RTriangle::getVectorTo(const RLine& / *line* /, bool / *limited* /) const {
	assert(false);
	return RVector();
}
*/


QList<RVector> RTriangle::getEndPoints() const {
    QList<RVector> c;

    c.append(corner[0]);
    c.append(corner[1]);
    c.append(corner[2]);

    return c;
}

QList<RVector> RTriangle::getMiddlePoints() const {
    QList<RVector> c;

    c.append((corner[0]+corner[1])/2.0);
    c.append((corner[1]+corner[2])/2.0);
    c.append((corner[2]+corner[0])/2.0);

    return c;
}

QList<RVector> RTriangle::getCenterPoints() const {
    return getMiddlePoints();
}

QList<RVector> RTriangle::getPointsWithDistanceToEnd(double distance) const {
    QList<RVector> c;

    RLine l1(corner[0], corner[1]);
    RLine l2(corner[1], corner[2]);
    RLine l3(corner[2], corner[0]);

    c.append(l1.getPointsWithDistanceToEnd(distance));
    c.append(l2.getPointsWithDistanceToEnd(distance));
    c.append(l3.getPointsWithDistanceToEnd(distance));

    return c;
}

/**
 * \sa http://en.wikipedia.org/wiki/Normal_vector
 * \sa http://en.wikipedia.org/wiki/Cross_product#Definition
 *
 * \return The normal vector of this triangle. The length of the normal
 * vector equals double the area of the triangle.
 */
RVector RTriangle::getNormal() const {
	return RVector::getCrossProduct(corner[0] - corner[2], corner[1]
			- corner[2]);
}

//bool RTriangle::intersectsWith(const RShape& other, bool limited) const {
//	QList<RVector> ip = getIntersectionPoints(other, limited);
//	return ip.size() > 0;
//}

/**
 * \todo implement for all shapes
 *
 */
//QList<RVector> RTriangle::getIntersectionPoints(const RShape& other,
//    bool limited) const {
//
//    QList<RVector> result;
//
//	const RLine* line = dynamic_cast<const RLine*> (&other);
//	if (line != NULL) {
//		RVector normal = getNormal();
//
//		if (normal.getMagnitude() < 1.0e-12) {
//			return result;
//		}
//
//		if ((line->endPoint - line->startPoint).getMagnitude() < 1.0e-12) {
//			return result;
//		}
//
//		double t = RVector::getDotProduct(normal, corner[2] - line->startPoint)
//				/ RVector::getDotProduct(normal, (line->endPoint
//						- line->startPoint));
//
//		// check if intersection point is on the line:
//		if (limited && (t < 0.0 || t > 1.0)) {
//			return result;
//		}
//
//		// intersection point:
//		RVector ip = line->startPoint + (line->endPoint - line->startPoint) * t;
//
//		// check if intersection point is inside the triangle:
//		if (!limited || isPointInTriangle(ip)) {
//			result.push_back(ip);
//		}
//		return result;
//	}
//
//	const RArc* arc = dynamic_cast<const RArc*> (&other);
//	if (arc != NULL) {
//		RTriangle plane(arc->getCenter(), arc->getStartPoint(),
//				arc->getEndPoint());
//		QList<RVector> r = plane.getIntersectionPoints(RLine(corner[0],
//				corner[1]));
//		r.append(plane.getIntersectionPoints(RLine(corner[1], corner[2])));
//		r.append(plane.getIntersectionPoints(RLine(corner[2], corner[0])));
//		qDebug() << "RTriangle::getIntersectionPoints: r:" << r;
//		RLine l(r[0], r[1]);
//		return l.getIntersectionPoints(*arc);
//	}
//
//    return result;
//}

/**
 * \return True if the given point is inside this triangle.
 *
 * \param ip the point
 *
 * \param treatAsQuadrant If \c treatAsQuadrant is true, the triangle is
 * treated as an open quadrant
 * or segment with its center at the first corner. In this case, the point is
 * inside, if it is on the plane that is limited by the two rays
 * corner[0] -> corner[1] and corner[0] -> corner[2].
 *
 */
bool RTriangle::isPointInTriangle(const RVector& ip, bool treatAsQuadrant) const {
	RVector normal = getNormal();

	RVector f;
	if (std::fabs(normal.x) > std::fabs(normal.y) && std::fabs(normal.x)
			> std::fabs(normal.z)) {
		// drop x component for inside test:
		f = RVector(0, 1, 1);
	} else if (std::fabs(normal.y) > std::fabs(normal.z)) {
		// drop y component for inside test:
		f = RVector(1, 0, 1);
	} else {
		// drop z component for inside test:
		f = RVector(1, 1, 0);
	}

	RVector p = ip.getMultipliedComponents(f);
	RVector a = corner[0].getMultipliedComponents(f);
	RVector b = corner[1].getMultipliedComponents(f);
	RVector c = corner[2].getMultipliedComponents(f);

	RVector v0 = c - a;
	RVector v1 = b - a;
	RVector v2 = p - a;

	double dot00 = RVector::getDotProduct(v0, v0);
	double dot01 = RVector::getDotProduct(v0, v1);
	double dot02 = RVector::getDotProduct(v0, v2);
	double dot11 = RVector::getDotProduct(v1, v1);
	double dot12 = RVector::getDotProduct(v1, v2);

	double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u > 0.0 && v > 0.0 && (treatAsQuadrant || u + v < 1.0));
}

bool RTriangle::isPointInQuadrant(const RVector& ip) const {
	return isPointInTriangle(ip, true);
}

/**
 * \return 'd' in the plane equation 'ax + by + cz = d'.
 */
double RTriangle::getD() const {
	RVector normal = getNormal();
	return -normal.x * corner[0].x - normal.y * corner[0].y - normal.z
			* corner[0].z;
}
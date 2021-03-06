/****************************************************************************
** $Id: rs_leader.h 6089 2007-08-21 21:03:27Z andrew $
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


#ifndef RS_LEADER_H
#define RS_LEADER_H

#include "rs_entity.h"
#include "rs_entitycontainer.h"



/**
 * Holds the data that defines a leader.
 */
class CAD_EXPORT RS_LeaderData {
public:
    RS_LeaderData() {}
    RS_LeaderData(bool arrowHeadFlag) {
        arrowHead = arrowHeadFlag;
    }

    /** true: leader has an arrow head. false: no arrow. */
    bool arrowHead;
};



/**
 * Class for a leader entity (kind of a polyline arrow).
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_Leader : public RS_EntityContainer {
public:
    RS_Leader(RS_EntityContainer* parentContainer=NULL);
    RS_Leader(RS_EntityContainer* parentContainer,
                const RS_LeaderData& d);
    virtual ~RS_Leader();

    virtual RS_Entity* clone() {
        RS_Leader* p = new RS_Leader(parentContainer, data);
        p->copyPropertiesFrom(this);
        p->entities = entities;
        p->entities.setAutoDelete(entities.autoDelete());
        p->initId();
        p->detach();
        return p;
    }

    /**    @return RS2::EntityDimLeader */
    virtual RS2::EntityType rtti() const {
        return RS2::EntityDimLeader;
    }
    virtual bool isOfType(RS2::EntityType t) const {
        return (t==RS2::EntityDimLeader || RS_EntityContainer::isOfType(t));
    }
    virtual RS_String typeName() const {
        return QObject::tr("Leader");
    }

    virtual void update();

    /** @return Copy of data that defines the leader. */
    RS_LeaderData getData() const {
        return data;
    }

    double getArrowSize();

    /** @return true: if this leader has an arrow at the beginning. */
    bool hasArrowHead() {
        return data.arrowHead;
    }

    virtual RS_Entity* addVertex(const RS_Vector& v);
    virtual void addEntity(RS_Entity* entity);

    virtual double getLength() {
        return -1.0;
    }
    virtual RS_VectorList getRefPoints(RS2::RefMode refMode=RS2::RefAll);
    virtual RS_Vector getNearestRef(const RS_Vector& coord,
                                     double* dist = NULL,
                                     RS2::RefMode refMode=RS2::RefAll);
    virtual RS_Vector getNearestSelectedRef(const RS_Vector& coord,
                                     double* dist = NULL,
                                     RS2::RefMode refMode=RS2::RefAll);
    
    void overrideArrowSize(double f) {
        arrowSize = f;
    }
    
    virtual void move(const RS_Vector& offset);
    virtual void rotate(const RS_Vector& center, double angle);
    virtual void scale(const RS_Vector& center, const RS_Vector& factor);
    virtual void mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2);
    virtual void moveRef(const RS_Vector& ref, const RS_Vector& offset);
    virtual void stretch(const RS_Vector& firstCorner,
                         const RS_Vector& secondCorner,
                         const RS_Vector& offset);

protected:
    RS_LeaderData data;
    bool empty;
    double arrowSize;
};

#endif

/****************************************************************************
** $Id: gv_pen.h 7877 2008-01-14 17:44:08Z andrew $
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
** not clear to you. Updated for Biluna 2016-08-11
**
**********************************************************************/


#ifndef GV_PEN_H
#define GV_PEN_H

#include "gv.h"
#include "gv_color.h"
#include "gv_flags.h"


/**
 * A pen stores attributes for painting such as line width,
 * linetype, color, ...
 *
 * @author Andrew Mustun
 */
class GV_Pen : public GV_Flags {
public:
    /**
     * Creates a default pen (black, solid, width 0).
     */
    GV_Pen() : GV_Flags() {
        setColor(GV_Color(0,0,0));
        setWidth(GV2::Width00);
        setLineType(GV2::SolidLine);
        setScreenWidth(0);
    }
    /**
     * Creates a pen with the given attributes.
     */
    GV_Pen(const GV_Color& c,
           GV2::LineWidth w,
           GV2::LineType t,
           double sw=0.0) : GV_Flags() {
        setColor(c);
        setWidth(w);
        setLineType(t);
        setScreenWidth(sw);
    }
    /**
     * Creates a default pen with the given flags. This is 
     * usually used to create invalid pens.
     *
     * e.g.:
     * <pre>
     *   GV_Pen p(GV2::FlagInvalid);
     * </pre>
     */
    GV_Pen(unsigned int f) : GV_Flags(f) {
        setColor(GV_Color(0,0,0));
        setWidth(GV2::Width00);
        setLineType(GV2::SolidLine);
        setScreenWidth(0);
    }
    //GV_Pen(const GV_Pen& pen) : GV_Flags(pen.getFlags()) {
    //    lineType = pen.lineType;
    //    width = pen.width;
    //    color = pen.color;
    //}
    virtual ~GV_Pen() {}

    GV2::LineType getLineType() const {
        return lineType;
    }
    void setLineType(GV2::LineType t) {
        lineType = t;
    }
    GV2::LineWidth getWidth() const {
        return width;
    }
    void setWidth(GV2::LineWidth w) {
        width = w;
    }
    double getScreenWidth() const {
        return screenWidth;
    }
    void setScreenWidth(double w) {
        screenWidth = w;
    }
    const GV_Color& getColor() const {
        return color;
    }
    void setColor(const GV_Color& c) {
        color = c;
    }
    bool isValid() {
        return !getFlag(GV2::FlagInvalid);
    }

    QString toString() {
        return QString("GV_Pen: lineType: %1, width: %2, "
                         "screenWidth: %3, color: %4")
                         .arg(lineType).arg(width)
                         .arg(screenWidth).arg(color.toString());
    }

    //void makeTransparent(int value) {
    //    color.setAlpha(value);
    //}

    //GV_Pen& operator = (const GV_Pen& p) {
    //    lineType = p.lineType;
    //    width = p.width;
    //    color = p.color;
    //    setFlags(p.getFlags());

    //    return *this;
    //}

    bool operator == (const GV_Pen& p) const {
        return (lineType==p.lineType && width==p.width && color==p.color);
    }

    bool operator != (const GV_Pen& p) const {
        return !(*this==p);
    }

protected:
    GV2::LineType lineType;
    GV2::LineWidth width;
    double screenWidth;
    GV_Color color;
};

#endif

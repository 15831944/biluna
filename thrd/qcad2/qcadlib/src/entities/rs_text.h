/****************************************************************************
** $Id: rs_text.h 7857 2008-01-12 19:23:21Z andrew $
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


#ifndef RS_TEXT_H
#define RS_TEXT_H

#include "rs_entity.h"
#include "rs_entitycontainer.h"

#define RS_TEXT_INSERTIONPOINT_X   QObject::tr(RS_Text::propertyInsertionPointX)
#define RS_TEXT_INSERTIONPOINT_Y   QObject::tr(RS_Text::propertyInsertionPointY)
#define RS_TEXT_HEIGHT             QObject::tr(RS_Text::propertyHeight)
#define RS_TEXT_VALIGN             QObject::tr(RS_Text::propertyVAlign)
#define RS_TEXT_HALIGN             QObject::tr(RS_Text::propertyHAlign)
#define RS_TEXT_LINESPACINGFACTOR  QObject::tr(RS_Text::propertyLineSpacingFactor)
#define RS_TEXT_TEXT               QObject::tr(RS_Text::propertyText)
#define RS_TEXT_STYLE              QObject::tr(RS_Text::propertyStyle)
#define RS_TEXT_ANGLE              QObject::tr(RS_Text::propertyAngle)

/**
 * Holds the data that defines a text entity.
 */
class CAD_EXPORT RS_TextData {
public:
    /**
     * Default constructor. Leaves the data object uninitialized.
     */
    RS_TextData() {}

    /**
     * Constructor with initialisation.
     *
     * @param insertionPoint Insertion point
     * @param height Nominal (initial) text height
     * @param width Reference rectangle width
     * @param valign Vertical alignment
     * @param halign Horizontal alignment
     * @param drawingDirection Drawing direction
     * @param lineSpacingStyle Line spacing style
     * @param lineSpacingFactor Line spacing factor
     * @param text Text string
     * @param style Text style name
     * @param angle Rotation angle
     * @param updateMode RS2::Update will update the text entity instantly
     *    RS2::NoUpdate will not update the entity. You can update
     *    it later manually using the update() method. This is
     *    often the case since you might want to adjust attributes
     *    after creating a text entity.
     */
    RS_TextData(const RS_Vector& insertionPoint,
                double height,
                double width,
                RS2::VAlign valign,
                RS2::HAlign halign,
                RS2::TextDrawingDirection drawingDirection,
                RS2::TextLineSpacingStyle lineSpacingStyle,
                double lineSpacingFactor,
                const RS_String& text,
                const RS_String& style,
                double angle,
                RS2::UpdateMode updateMode = RS2::Update) {
        this->insertionPoint = insertionPoint;
        this->height = height;
        this->width = width;
        this->valign = valign;
        this->halign = halign;
        this->drawingDirection = drawingDirection;
        this->lineSpacingStyle = lineSpacingStyle;
        this->lineSpacingFactor = lineSpacingFactor;
        this->style = style;
        this->angle = angle;
        this->text = text;
        this->updateMode = updateMode;
    }

    friend class RS_Text;

public:
    /** Insertion point */
    RS_Vector insertionPoint;
    /** Nominal (initial) text height */
    double height;
    /** Reference rectangle width */
    double width;
    /** Vertical alignment */
    RS2::VAlign valign;
    /** Horizontal alignment */
    RS2::HAlign halign;
    /** Drawing direction */
    RS2::TextDrawingDirection drawingDirection;
    /** Line spacing style */
    RS2::TextLineSpacingStyle lineSpacingStyle;
    /** Line spacing factor */
    double lineSpacingFactor;
    /** Text string */
    RS_String text;
    /** Text style name */
    RS_String style;
    /** Rotation angle */
    double angle;
    /** Update mode */
    RS2::UpdateMode updateMode;
};



/**
 * Class for a text entity.
 * Please note that text strings can contain special 
 * characters such as %%c for a diameter sign as well as unicode
 * characters. Line feeds are stored as real line feeds in the string.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_Text : public RS_EntityContainer {
public:
    RS_Text(RS_EntityContainer* parentContainer,
            const RS_TextData& d);
    virtual ~RS_Text() {}

    virtual RS_Entity* clone() {
        RS_Text* t = new RS_Text(parentContainer, data);
        t->copyPropertiesFrom(this);
        t->usedTextWidth = usedTextWidth;
        t->usedTextHeight = usedTextHeight;
        t->lineInsertionPoints = lineInsertionPoints;
        t->entities = entities;
        t->entities.setAutoDelete(entities.autoDelete());
        t->initId();
        t->detach();
        return t;
    }

    /**    @return RS2::EntityText */
    virtual RS2::EntityType rtti() const {
        return RS2::EntityText;
    }
    virtual bool isOfType(RS2::EntityType t) const {
        return (t==RS2::EntityText || RS_EntityContainer::isOfType(t));
    }
    virtual RS_String typeName() const {
        return QObject::tr("Text");
    }

    /** @return Copy of data that defines the text. */
    RS_TextData getData() const {
        return data;
    }

    virtual void calculateBorders(bool visibleOnly=false);
    void update();
    void updateAddLine(RS_EntityContainer* textLine, int lineCounter);

    int getNumberOfLines();

    RS_Vector getInsertionPoint() {
        return data.insertionPoint;
    }
    void setInsertionPoint(const RS_Vector& ip) {
        data.insertionPoint = ip;
    }
    double getHeight() {
        return data.height;
    }
    void setHeight(double h) {
        data.height = h;
    }
    double getWidth() {
        return data.width;
    }
    void setAlignment(int a);
    int getAlignment();
    RS2::VAlign getVAlign() {
        return data.valign;
    }
    void setVAlign(RS2::VAlign va) {
        data.valign = va;
    }
    RS2::HAlign getHAlign() {
        return data.halign;
    }
    void setHAlign(RS2::HAlign ha) {
        data.halign = ha;
    }
    RS2::TextDrawingDirection getDrawingDirection() {
        return data.drawingDirection;
    }
    RS2::TextLineSpacingStyle getLineSpacingStyle() {
        return data.lineSpacingStyle;
    }
    void setLineSpacingFactor(double f) {
        data.lineSpacingFactor = f;
    }
    double getLineSpacingFactor() {
        return data.lineSpacingFactor;
    }
    void setText(const RS_String& t);
    RS_String getText() {
        return data.text;
    }
    void setStyle(const RS_String& s) {
        data.style = s;
    }
    RS_String getStyle() {
        return data.style;
    }
    void setAngle(double a) {
        data.angle = a;
    }
    double getAngle() {
        return data.angle;
    }
    double getUsedTextWidth() {
        return usedTextWidth;
    }
    double getUsedTextHeight() {
        return usedTextHeight;
    }

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
    
    virtual RS_VectorList getLineInsertionPoints();

    virtual void move(const RS_Vector& offset);
    virtual void rotate(const RS_Vector& center, double angle);
    virtual void scale(const RS_Vector& center, const RS_Vector& factor);
    virtual void mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2);
    virtual void moveRef(const RS_Vector& ref, const RS_Vector& offset);
    virtual bool hasEndpointsWithinWindow(const RS_Vector& v1, const RS_Vector& v2);
    virtual void stretch(const RS_Vector& firstCorner,
                         const RS_Vector& secondCorner,
                         const RS_Vector& offset);

    virtual RS_Variant getProperty(const RS_String& name, 
        const RS_Variant& def=RS_Variant());
    virtual RS_StringList getPropertyNames(bool includeGeometry=true);
    virtual void setProperty(const RS_String& name, const RS_Variant& value);
    //virtual RS_StringList getPropertyChoices(const RS_String& name);
    virtual RS_PropertyAttributes getPropertyAttributes(
        const RS_String& propertyName);

    static RS_String vAlignToText(RS2::VAlign va) {
        switch(va) {
        default:
        case RS2::VAlignTop:
            return QObject::tr("Top");
        case RS2::VAlignMiddle:
            return QObject::tr("Middle");
        case RS2::VAlignBottom:
            return QObject::tr("Bottom");
        }
    }
    
    static RS2::VAlign textToVAlign(const RS_String& t) {
        if (t==vAlignToText(RS2::VAlignTop)) {
            return RS2::VAlignTop;
        }
        else if (t==vAlignToText(RS2::VAlignMiddle)) {
            return RS2::VAlignMiddle;
        }
        else if (t==vAlignToText(RS2::VAlignBottom)) {
            return RS2::VAlignBottom;
        }
        
        return RS2::VAlignTop;
    }
    
    static RS_String hAlignToText(RS2::HAlign va) {
        switch(va) {
        default:
        case RS2::HAlignLeft:
            return QObject::tr("Left");
        case RS2::HAlignCenter:
            return QObject::tr("Center");
        case RS2::HAlignRight:
            return QObject::tr("Right");
        }
    }
    
    static RS2::HAlign textToHAlign(const RS_String& t) {
        if (t==hAlignToText(RS2::HAlignLeft)) {
            return RS2::HAlignLeft;
        }
        else if (t==hAlignToText(RS2::HAlignCenter)) {
            return RS2::HAlignCenter;
        }
        else if (t==hAlignToText(RS2::HAlignRight)) {
            return RS2::HAlignRight;
        }

        return RS2::HAlignLeft;
    }

protected:
    RS_TextData data;

    /**
     * Text width used by the current contents of this text entity. 
     * This property is updated by the update method.
     * @see update
     */
    double usedTextWidth;
    /**
     * Text height used by the current contents of this text entity.
     * This property is updated by the update method.
     * @see update
     */
    double usedTextHeight;

    /**
     * Line insertion points. One for every text line. Used to split up 
     * text into text lines.
     */
    RS_VectorList lineInsertionPoints;

    static const char* propertyInsertionPointX;
    static const char* propertyInsertionPointY;
    static const char* propertyHeight;
    static const char* propertyVAlign;
    static const char* propertyHAlign;
    static const char* propertyLineSpacingFactor;
    static const char* propertyText;
    static const char* propertyStyle;
    static const char* propertyAngle;
};

#endif

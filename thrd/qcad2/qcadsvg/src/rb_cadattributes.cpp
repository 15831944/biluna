/****************************************************************************
** $Id: rb_cadattributes.cpp, 2007/11/17 8:51:48 rutger Exp $
 *
 * Copyright (C) 2004-2005 Red-Bag. All rights reserved.
 * This file is part of the qcadsvg project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "rb_cadattributes.h"


RB_CadAttributes::RB_CadAttributes() {
    RS_Color tempColor(128,128,128);

    setLayer("");
    setColor(tempColor);
    setWidth(0.05);
    setLineType(RS2::SolidLine);
}



/**
 * Constructor for XML attributes.
 * @param layerName Layer name for this entity or NULL for no layer
 *              (every entity should be on a named layer!).
 * @param blockName Block name for this entity or NULL for no block
 * @param strokeColor Color number (0..256). 0 = BYBLOCK, 256 = BYLAYER.
 * @param width Line thickness. Defaults to zero.
 * @param lineType Line type name or "BYLAYER" or "BYBLOCK". Defaults
 *              to "BYLAYER"
 * @param textColor fill color for text
 */
RB_CadAttributes::RB_CadAttributes(const QString& layerName, 
									const QString& blockName,
              						const RS_Color& color, double width,
              						RS2::LineType lineType,
              						bool validAttrib) {
    setLayer(layerName);
    setBlock(blockName);
    setColor(color);
    setWidth(width);
    setLineType(lineType);
    setValid(validAttrib);
}


void RB_CadAttributes::reset() {
    setLayer("");
    setBlock("");
    setColor(RS_Color());
    setWidth(0.05);
    setLineType(RS2::SolidLine);
}


/**
 * Qcad line type to SVG dashArray type
 *
 * @returns string with dash dot size combination
 * todo: check for nonvalid line widths
 */
QString RB_CadAttributes::getSvgLineType() const {
    // Note: the dot length is depending the stroke width, 
    //   this makes the 'blank' length also depending the stroke width 
    QString dashArray = "";
    QString str = "";
    double dashLength = 0.0;
    double dotLength = 0.0;
    double spaceLength = 0.0;
    double mWidth = 0.0;
    
    // set mWidth for the case of the default line thickness
    if (1.0e-10 < getWidth()) {
        mWidth = getWidth();
    } else {
        mWidth = 0.05;
    }
    
    switch (lineType) {
        case RS2::DotLine :
            // Dot (as in QCad) in SVG e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="0.2 6.3" x1="-100" x2="0" y1="-90" y2="-90" />
            dotLength = 0;
            spaceLength = 6.25 * mWidth;
            dashArray = str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DotLine2 :
            // Dot (small) e.g.: <line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="0.2 3.0" x1="-100" x2="0" y1="-80" y2="-80" />
            dotLength = 0;
            spaceLength = 3.125 * mWidth;
            dashArray = str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DotLineX2 :
            // Dot (large) e.g.: <line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="0.2 13.7" x1="-100" x2="0" y1="-70" y2="-70" />
            dotLength = 0;
            spaceLength = 12.5 * mWidth;
            dashArray = str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DashLine :
            // Dash e.g.:<line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="13.4 6.6" x1="-100" x2="0" y1="-60" y2="-60" />
            dashLength = 12 * mWidth;
            spaceLength = 6 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DashLine2 :
            // Dash (small) e.g.:<line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="6.7 6.6" x1="-100" x2="0" y1="-60" y2="-60" />
            dashLength = 6 * mWidth;
            spaceLength = 3 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DashLineX2 :
            // Dash (large) e.g.:<line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="26.8 6.6" x1="-100" x2="0" y1="-60" y2="-60" />
            dashLength = 24 * mWidth;
            spaceLength = 12 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DashDotLine :
            // Dash Dot e.g.:<line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="13.4 6.5 0.2 6.5" x1="-100" x2="0" y1="-60" y2="-60" />
            dashLength = 12 * mWidth;
            dotLength = 0;
            spaceLength = 6 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength);
            break;
        case RS2::DashDotLine2 :
            // Dash Dot (small) e.g.:<line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="6.6 3.2 0.2 3.2" x1="-100" x2="0" y1="-60" y2="-60" />
            dashLength = 6 * mWidth;
            dotLength = 0;
            spaceLength = 3 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength);                break;
        case RS2::DashDotLineX2 :
            // Dash Dot (large) e.g.:<line stroke="blue" stroke-width="0.2"
            //   stroke-dasharray="26.8 13.1 0.2 13.1" x1="-100" x2="0" y1="-60" y2="-60" />
            dashLength = 24 * mWidth;
            dotLength = 0;
            spaceLength = 12 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength);
            break;
        case RS2::DivideLine :
            // Divide e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="12.8 6.0 0.2 6.0 0.2 6.0" x1="0" x2="100" y1="-90" y2="-90" />
            dashLength = 12 * mWidth;
            dotLength = 0;
            spaceLength = 6 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DivideLine2 :
            // Divide (small) e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="6.6 2.8 0.2 2.8 0.2 2.8" x1="0" x2="100" y1="-90" y2="-90" />
            dashLength = 6 * mWidth;
            dotLength = 0;
            spaceLength = 3 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::DivideLineX2 :
            // Divide (large) e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="26.9 13.1 0.2 13.1 0.2 13.1" x1="0" x2="100" y1="-90" y2="-90" />
            dashLength = 24 * mWidth;
            dotLength = 0;
            spaceLength = 12 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::CenterLine :
            // Center e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="40 6 6 6" x1="0" x2="100" y1="-60" y2="-60" />
            dashLength = 32 * mWidth;
            dotLength = 6 * mWidth;
            spaceLength = 6 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength);
            break;
        case RS2::CenterLine2 :
            // Center (small) e.g.:<line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="20 3 3 3" x1="0" x2="100" y1="-50" y2="-50" />
            dashLength = 16 * mWidth;
            dotLength = 3 * mWidth;
            spaceLength = 3 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength);
            break;
        case RS2::CenterLineX2 :
            // Center (large) e.g.:<line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="80 12 12 12" x1="0" x2="100" y1="-50" y2="-50" />
            dashLength = 64 * mWidth;
            dotLength = 12 * mWidth;
            spaceLength = 12 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength)
                + "," + str.setNum(spaceLength);
            break;
        case RS2::BorderLine :
            // Border e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="13.4 6.5 13.4 6.5 0.2 6.5" x1="0" x2="100" y1="-30" y2="-30" />
            dashLength = 12 * mWidth;
            dotLength = 0;
            spaceLength = 6 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dashLength)
                + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::BorderLine2 :
            // Border (small) e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="6.6 3.2 6.6 3.2 0.2 3.2" x1="0" x2="100" y1="-30" y2="-30" />
            dashLength = 6 * mWidth;
            dotLength = 0;
            spaceLength = 3 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dashLength)
                + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        case RS2::BorderLineX2 :
            // Border (large) e.g.: <line stroke="blue" stroke-width="0.2" 
            //   stroke-dasharray="26.8 13.1 26.8 13.1 0.2 13.1" x1="0" x2="100" y1="-30" y2="-30" />
            dashLength = 24 * mWidth;
            dotLength = 0;
            spaceLength = 12 * mWidth;
            dashArray = str.setNum(dashLength) + "," + str.setNum(spaceLength) + "," + str.setNum(dashLength)
                + "," + str.setNum(spaceLength) + "," + str.setNum(dotLength) + "," + str.setNum(spaceLength);
            break;
        default  :
            dashArray = "none";
            break;
    }
    return dashArray;
}

/**
 * QCad line type names to number for getSvgLineType()
 * / TODO delete not required anymore
int RB_CadAttributes::getIntLineType(const QString& lineType) const {
    
    if ("CONTINUOUS" == lineType) {
        return 1;
    } else if ("DOT" == lineType) {
        return 2;     
    } else if ("DOT2" == lineType) {
        return 3;     
    } else if ("DOTX2" == lineType) {
        return 4;     
    } else if ("DASHED" == lineType) {
        return 5;     
    } else if ("DASHED2" == lineType) {
        return 6;     
    } else if ("DASHEDX2" == lineType) {
        return 7;     
    } else if ("DASHDOT" == lineType) {
        return 8;     
    } else if ("DASHDOT2" == lineType) {
        return 9;     
    } else if ("DASHDOTX2" == lineType) {
        return 10;     
    } else if ("DIVIDE" == lineType) {
        return 11;     
    } else if ("DIVIDE2" == lineType) {
        return 12;     
    } else if ("DIVIDEX2" == lineType) {
        return 13;     
    } else if ("CENTER" == lineType) {
        return 14;     
    } else if ("CENTER2" == lineType) {
        return 15;     
    } else if ("CENTERX2" == lineType) {
        return 16;     
    } else if ("BORDER" == lineType) {
        return 17;     
    } else if ("BORDER2" == lineType) {
        return 18;     
    } else if ("BORDERX2" == lineType) {
        return 19;     
    } else if ("BYLAYER" == lineType) {
        return -1;     
    } else if ("BYBlOCK" == lineType) {
        return -2;     
    }   
    return 1;
}
*/

/**
 * Sets the layer. If the given pointer points to NULL, the
 *  new layer name will be an empty but valid string.
 */
void RB_CadAttributes::setLayer(const QString& lName) {
    this->layerName = lName;
}



/**
 * @return Layer name.
 */
QString RB_CadAttributes::getLayer() const {
    return layerName;
}

/**
 * Sets the block name. If the given pointer points to NULL, the
 * new layer name will be an empty but valid string.
 */
void RB_CadAttributes::setBlock(const QString& bName) {
    this->blockName = bName;
}



/**
 * @return block name.
 */
QString RB_CadAttributes::getBlock() const {
    return blockName;
}


/**
 * Sets the stroke color, color for most entities except text
 */
void RB_CadAttributes::setColor(RS_Color c) {
    this->color = c;
}


/**
 * @return the color.
 */
RS_Color RB_CadAttributes::getColor() const {
    return color;
}


/**
 * @return Color in "rgb(r,g,b)"
 */
QString RB_CadAttributes::getSvgColor() const {
    RB_SvgCodes svgCodes;
	return svgCodes.replaceBySvgColor(this->color);
}

/**
 * Sets the width.
 */
void RB_CadAttributes::setWidth(double width) {
    this->width = width;
}



/**
 * @return Width.
 */
double RB_CadAttributes::getWidth() const {
    return width;
}



/**
 * @return SVG Width.
 */
double RB_CadAttributes::getSvgWidth() const {
    return width;
}



/**
 * Sets the line type. This can be any string and is not
 *  checked to be a valid line type. 
 */
void RB_CadAttributes::setLineType(RS2::LineType lineType) {
    this->lineType = lineType;
}



/**
 * @return Line type.
 */
RS2::LineType RB_CadAttributes::getLineType() const {
    return lineType;
}

/**
 * Indicate whether these attributes are concerning a text entity
 * @param textAttrib true is for a text entity, false is for other entities 
 */
void RB_CadAttributes::setValid(bool validAttrib) {
	this->validAttribute = validAttrib;
}

/**
 * @return whether these attributes are for a text entity
 */
bool RB_CadAttributes::isValid() const {
	return validAttribute;
}

/**
 * Copies attributes (deep copies) from another attribute object.
 */
RB_CadAttributes& RB_CadAttributes::operator= (const RB_CadAttributes& attrib) {
    setLayer(attrib.getLayer());
    setBlock(attrib.getBlock());
	setColor(attrib.getColor());
    setWidth(attrib.getWidth());
    setLineType(attrib.getLineType());
    setValid(attrib.isValid());
    return *this;
}

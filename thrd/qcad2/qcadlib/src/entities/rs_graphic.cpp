/****************************************************************************
** $Id: rs_graphic.cpp 7930 2008-01-17 21:23:04Z andrew $
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


#include "rs_graphic.h"

#include "rs_debug.h"
#include "rs_fileio.h"
#include "rs_math.h"
#include "rs_units.h"
#include "rs_settings.h"


/**
 * Default constructor.
 */
RS_Graphic::RS_Graphic(RS_EntityContainer* parentContainer)
        : RS_Entity(parentContainer), RS_Document(parentContainer),
        layerList(), blockList(true)
#ifdef RS_CAM
, camData()
#endif
{

    RS_SETTINGS->beginGroup("/Defaults");
    setUnit(RS_Units::stringToUnit(RS_SETTINGS->readEntry("/Unit", "None")));
    RS_SETTINGS->endGroup();

    RS2::Unit unit = getUnit();

    if (unit==RS2::Inch) {
        addVariable("$DIMASZ", 0.1, 40);
        addVariable("$DIMEXE", 0.05, 40);
        addVariable("$DIMEXO", 0.025, 40);
        addVariable("$DIMGAP", 0.025, 40);
        addVariable("$DIMTXT", 0.1, 40);
    } else {
        addVariable("$DIMASZ",
                    RS_Units::convert(2.5, RS2::Millimeter, unit), 40);
        addVariable("$DIMEXE",
                    RS_Units::convert(1.25, RS2::Millimeter, unit), 40);
        addVariable("$DIMEXO",
                    RS_Units::convert(0.625, RS2::Millimeter, unit), 40);
        addVariable("$DIMGAP",
                    RS_Units::convert(0.625, RS2::Millimeter, unit), 40);
        addVariable("$DIMTXT",
                    RS_Units::convert(2.5, RS2::Millimeter, unit), 40);
    }
    setModified(false);
    setModifiedSinceRedraw(true);
}



/**
 * Destructor.
 */
RS_Graphic::~RS_Graphic() {
    RS_DEBUG->print("RS_Graphic::~RS_Graphic");
    RS_DEBUG->print("RS_Graphic::~RS_Graphic: OK");
}



/**
 * Counts the entities on the given layer.
 */
int RS_Graphic::countLayerEntities(RS_Layer* layer) {

    int c=0;

    if (layer!=NULL) {
        for (RS_Entity* t=firstEntity(RS2::ResolveNone);
                t!=NULL;
                t=nextEntity(RS2::ResolveNone)) {

            if (t->getLayer()!=NULL &&
                    t->getLayer()->getName()==layer->getName()) {
                c+=t->countDeep();
            }
        }
    }

    return c;
}



/**
 * Removes the given layer and undoes all entities on it.
 */
void RS_Graphic::removeLayer(RS_Layer* layer) {

    if (layer!=NULL && layer->getName()!="0") {

        // remove all entities on that layer:
        startUndoCycle();
        for (RS_Entity* e=firstEntity(RS2::ResolveNone);
                e!=NULL;
                e=nextEntity(RS2::ResolveNone)) {

            if (e->getLayer()!=NULL &&
                    e->getLayer()->getName()==layer->getName()) {

                e->setUndoState(true);
                e->setLayer("0");
                addUndoable(e);
            }
        }
        endUndoCycle();

        // remove all entities in blocks that are on that layer:
        for (int bi=0; bi<blockList.count(); bi++) {
            RS_Block* blk = blockList.at(bi);

            if (blk!=NULL) {
                for (RS_Entity* e=blk->firstEntity(RS2::ResolveNone);
                        e!=NULL;
                        e=blk->nextEntity(RS2::ResolveNone)) {

                    if (e->getLayer()!=NULL &&
                            e->getLayer()->getName()==layer->getName()) {

                        e->setUndoState(true);
                        e->setLayer("0");
                        //addUndoable(e);
                    }
                }
            }
        }

        layerList.remove(layer);
    }
}


/**
 * Clears all layers, blocks and entities of this graphic.
 * A default layer (0) is created.
 */
void RS_Graphic::newDoc() {

    RS_DEBUG->print("RS_Graphic::newDoc");

    clear();

    clearLayers();
    clearBlocks();

    addLayer(
        new RS_Layer(
            RS_LayerData(
                "0", 
                RS_Pen(RS_Color(0,0,0), RS2::Width07, RS2::SolidLine), 
                false, 
                false
            )
        )
    );
    //addLayer(new RS_Layer("ByBlock"));

    setModified(false);
    setModifiedSinceRedraw(true);
    
    RS_DEBUG->print("RS_Graphic::newDoc: OK");
}



/**
 * Saves this graphic with the current filename and settings.
 */
bool RS_Graphic::save() {

    bool ret = false;
    
    RS_DEBUG->print("RS_Graphic::save");
    RS_DEBUG->print("  file: %s", (const char*)filename.toLatin1());
    RS_DEBUG->print("  format: %d", (int)formatType);

    RS_DEBUG->print("  export...");
    ret = RS_FILEIO->fileExport(*this, filename, formatType);

    if (ret) {
        setModified(false);
        setModifiedSinceRedraw(true);
        layerList.setModified(false);
        blockList.setModified(false);
    }

    RS_DEBUG->print("RS_Graphic::save ok");

    return ret;
}



/**
 * Saves this graphic with the given filename and current settings.
 */
bool RS_Graphic::saveAs(const RS_String &filename, RS2::FormatType type) {

    RS_DEBUG->print("RS_Graphic::saveAs");

    this->filename = filename;
    this->formatType = type;

    return save();
}



/**
 * Loads the given fils into this graphic.
 */
bool RS_Graphic::open(const RS_String &filename, RS2::FormatType type, bool clean) {
    RS_DEBUG->print("RS_Graphic::open(%s)", (const char*)filename.toLatin1());

    bool ret = false;

    this->filename = filename;
    this->originalFilename = filename;

    // clean all:
    if (clean) {
        newDoc();
    }

    // import file:
    ret = RS_FILEIO->fileImport(*this, filename, type);
        
    if (clean) {
        setModified(false);
        setModifiedSinceRedraw(true);
        layerList.setModified(false);
        blockList.setModified(false);
    }

    RS_DEBUG->print("RS_Graphic::open(%s): OK", (const char*)filename.toLatin1());

    return ret;
}



/**
 * @return true if the grid is switched on (visible).
 */
bool RS_Graphic::isGridOn() {
    int on = getVariableInt("$GRIDMODE", 1);
    bool ret = (on!=0);
    RS_DEBUG->print("RS_Graphic::isGridOn: id: %d, ret: %d", getId(), (int)ret);
    return ret;
}



/**
 * Enables / disables the grid.
 */
void RS_Graphic::setGridOn(bool on) {
    RS_DEBUG->print("RS_Graphic::setGridOn: id: %d, on: %d", getId(), (bool)on);
    addVariable("$GRIDMODE", (int)on, 70);
}

/**
 * Toggles the grid.
 */
/*void RS_Graphic::toggleGrid() {
    RS_DEBUG->print("RS_Graphic::toggleGrid: id: %d", getId());
    setGridOn(!isGridOn());
}*/



/**
 * Sets the unit of this graphic to 'u'
 */
void RS_Graphic::setUnit(RS2::Unit u) {

    setPaperSize(RS_Units::convert(getPaperSize(), getUnit(), u));

    addVariable("$INSUNITS", (int)u, 70);

    //unit = u;
}



/**
 * Gets the unit of this graphic 
 */
RS2::Unit RS_Graphic::getUnit() {
    return (RS2::Unit)getVariableInt("$INSUNITS", 0);
    //return unit;
}



/**
 * @return The linear format type for this document.
 * This is determined by the variable "$LUNITS".
 */
RS2::LinearFormat RS_Graphic::getLinearFormat() {
    int lunits = getVariableInt("$LUNITS", 2);

    switch (lunits) {
    default:
    case 2:
        return RS2::Decimal;
        break;

    case 1:
        return RS2::Scientific;
        break;

    case 3:
        return RS2::Engineering;
        break;

    case 4:
        return RS2::Architectural;
        break;

    case 5:
        return RS2::Fractional;
        break;
    }

    return RS2::Decimal;
}



/**
 * @return The linear precision for this document.
 * This is determined by the variable "$LUPREC".
 */
int RS_Graphic::getLinearPrecision() {
    return getVariableInt("$LUPREC", 4);
}

    
    
bool RS_Graphic::showLeadingZeroes() {
    return !(getVariableInt("$DIMZIN", 8) & 4);
}


bool RS_Graphic::showTrailingZeroes() {
    return !(getVariableInt("$DIMZIN", 8) & 8);
}

bool RS_Graphic::showLeadingZeroesAngle() {
    return !(getVariableInt("$DIMAZIN", 8) & 4);
}


bool RS_Graphic::showTrailingZeroesAngle() {
    return !(getVariableInt("$DIMAZIN", 8) & 8);
}


/**
 * @return The angle format type for this document.
 * This is determined by the variable "$AUNITS".
 */
RS2::AngleFormat RS_Graphic::getAngleFormat() {
    int aunits = getVariableInt("$AUNITS", 0);

    switch (aunits) {
    default:
    case 0:
        return RS2::DegreesDecimal;
        break;

    case 1:
        return RS2::DegreesMinutesSeconds;
        break;

    case 2:
        return RS2::Gradians;
        break;

    case 3:
        return RS2::Radians;
        break;

    case 4:
        return RS2::Surveyors;
        break;
    }

    return RS2::DegreesDecimal;
}



/**
 * @return The linear precision for this document.
 * This is determined by the variable "$LUPREC".
 */
int RS_Graphic::getAnglePrecision() {
    return getVariableInt("$AUPREC", 2);
}



/**
 * @return The insertion point of the drawing into the paper space.
 * This is the distance from the lower left paper edge to the zero
 * point of the drawing. DXF: $PINSBASE.
 */
RS_Vector RS_Graphic::getPaperInsertionBase() {
    return getVariableVector("$PINSBASE", RS_Vector(0.0,0.0));
}


/**
 * Sets the PINSBASE variable.
 */
void RS_Graphic::setPaperInsertionBase(const RS_Vector& p) {
    addVariable("$PINSBASE", p, 10);
}


/**
 * @return Paper size in graphic units.
 */
RS_Vector RS_Graphic::getPaperSize() {
    RS_Vector def = RS_Units::convert(RS_Vector(210.0,297.0),
                                      RS2::Millimeter, getUnit());

    RS_Vector v1 = getVariableVector("$PLIMMIN", RS_Vector(0.0,0.0));
    RS_Vector v2 = getVariableVector("$PLIMMAX", def);

    return v2-v1;
}


/**
 * Sets a new paper size.
 */
void RS_Graphic::setPaperSize(const RS_Vector& s) {
    addVariable("$PLIMMIN", RS_Vector(0.0,0.0), 10);
    addVariable("$PLIMMAX", s, 10);
}



/**
 * @return Paper format.
 * This is determined by the variables "$PLIMMIN" and "$PLIMMAX".
 *
 * @param landscape will be set to true for landscape and false for portrait if not NULL.
 */
RS2::PaperFormat RS_Graphic::getPaperFormat(bool* landscape) {
    RS_Vector size = RS_Units::convert(getPaperSize(),
                                       getUnit(), RS2::Millimeter);

    if (landscape!=NULL) {
        *landscape = (size.x>size.y);
    }

    return RS_Units::paperSizeToFormat(size);
}



/**
 * Sets the paper format to the given format.
 */
void RS_Graphic::setPaperFormat(RS2::PaperFormat f, bool landscape) {
    RS_Vector size = RS_Units::paperFormatToSize(f);

    if (landscape) {
        double tmp = size.x;
        size.x = size.y;
        size.y = tmp;
    }

    if (f!=RS2::Custom) {
        setPaperSize(RS_Units::convert(size, RS2::Millimeter, getUnit()));
    }
}



/**
 * @return Paper space scaling (DXF: $PSVPSCALE).
 */
double RS_Graphic::getPaperScale() {
    double ret;

    ret = getVariableDouble("$PSVPSCALE", 1.0);
    if (ret<1.0e-6) {
        ret = 1.0;
    }

    return ret;
}



/**
 * Sets a new scale factor for the paper space.
 */
void RS_Graphic::setPaperScale(double s) {
    addVariable("$PSVPSCALE", s, 40);
}



/**
 * Centers drawing on page. Affects DXF variable $PINSBASE.
 */
void RS_Graphic::centerToPage() {
    RS_Vector size = getPaperSize();

    double scale = getPaperScale();

    RS_Vector pinsbase = (size-getSize()*scale)/2.0 - getMin()*scale;

    setPaperInsertionBase(pinsbase);
}



/**
 * Fits drawing on page. Affects DXF variable $PINSBASE.
 */
void RS_Graphic::fitToPage() {
    double border = RS_Units::convert(25.0, RS2::Millimeter, getUnit());
    RS_Vector ps = getPaperSize() - RS_Vector(border, border);
    RS_Vector s = getSize();
    double fx = RS_MAXDOUBLE;
    double fy = RS_MAXDOUBLE;
    //double factor = 1.0;

    //ps = RS_Units::convert(ps, getUnit(), RS2::Millimeter);

    if (fabs(s.x)>1.0e-6) {
        fx = ps.x / s.x;
    }
    if (fabs(s.y)>1.0e-6) {
        fy = ps.y / s.y;
    }

    setPaperScale(std::min(fx, fy));
    centerToPage();
}


/**
 * @retval true The document has been modified since it was last saved.
 * @retval false The document has not been modified since it was last saved.
 */
bool RS_Graphic::isModified() const {
    return RS_Document::isModified() ||
        layerList.isModified() || blockList.isModified();
}

/**
 * Sets the documents modified status to 'm'.
 */
void RS_Graphic::setModified(bool m) {
    RS_Document::setModified(m);
    layerList.setModified(m);
    blockList.setModified(m);
}

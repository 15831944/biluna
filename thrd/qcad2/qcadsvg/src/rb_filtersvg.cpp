/*****************************************************************
 * $Id: rb_filtersvg.cpp,v 1.61 2004/09/18 23:52:21 - rutger $
 *
 * Copyright (C) 2004-2005 Red-Bag. All rights reserved.
 * This file is part of the PNID project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/


#include "rb_filtersvg.h"

#include <stdio.h>

#include "rb_cadattributes.h"
#include "rb_xmlcodes.h"
#include "rb_xmlwriter.h"

#include "rs_dimaligned.h"
#include "rs_dimangular.h"
#include "rs_dimdiametric.h"
#include "rs_dimlinear.h"
#include "rs_dimradial.h"
#include "rs_fileinfo.h"
#include "rs_hatch.h"
#include "rs_image.h"
#include "rs_leader.h"
#include "rs_regexp.h"
#include "rs_system.h"

#include <qtextcodec.h>


/**
 * Default constructor.
 */
RB_FilterSvg::RB_FilterSvg() {
    RS_DEBUG->print("RB_FilterSVG::RB_FilterSVG()");

    mtext = "";
    polyline = NULL;
    leader = NULL;
    hatch = NULL;
    hatchLoop = NULL;
    currentContainer = NULL;
    graphic = NULL;
    
    CDATAsection = false;
    currentContent = "";
    parseMessage = "";
    
    addImportFormat(RS2::FormatSvg);
    addExportFormat(RS2::FormatSvg);
}


/**
 * Destructor.
 */
RB_FilterSvg::~RB_FilterSvg() {
    RS_DEBUG->print("RB_FilterSVG::~RB_FilterSVG()");
}

/**
 * Import of SVG file
 * @param g QCad graphic container
 * @param file name of file to be imported
 * @param type file type to be saved, this filter is only for SVG file type
 * @return success 
 */
bool RB_FilterSvg::fileImport(RS_Graphic& g, const RS_String& file, 
    									RS2::FormatType type) { 
    RS_DEBUG->print("RB_FilterSVG::fileImport: importing file '%s'...", 
   									(const char*)QFile::encodeName(file));
    graphic = &g;
    currentContainer = graphic; // TODO: copied from DXF filter, required?
    this->file = file;
    QFile fn(file);
    
    // refer to QXmlReader for other functions
	reader.setContentHandler(this);
	reader.setErrorHandler(this);
	reader.setLexicalHandler(this);

    if (!reader.parse(QXmlInputSource(&fn))) {
		return false;	
	}

    return true; 
}

/**
 * Test comment section
 * /
bool RB_FilterSvg::comment(const QString& ch) {
std::cout<<"RB_FilterSVG::comment()"<<std::endl;
}
*/
/**
 * Start reading <![CDATA[ ... ]]> section
 */
bool RB_FilterSvg::startCDATA() {
	RS_DEBUG->print("RB_FilterSVG::startCDATA()");
	CDATAsection = true;
	return true;
}

/**
 * End reading <![CDATA[ ... ]]> section
 */
bool RB_FilterSvg::endCDATA() {
	RS_DEBUG->print("RB_FilterSVG::endCDATA()");

	if (!CDATAsection || currentContent.isEmpty()) return false;
	
	/*
		<style type="text/css">
		    <![CDATA[
		    g.LAYER-0 {opacity:1.0;
		    fill:none;
		    font-family:helvetica;
		    font-stretch:extra-condensed;
		    stroke-linecap:round;
		    stroke-linejoin:round;
		    stroke:rgb(0,0,0);
		    stroke-opacity:1.0;
		    stroke-width:0.2;
		    stroke-dasharray:0.6 0.6 0.6 0.6;}
		    g.LAYER-FRAME025 {opacity:1.0;
		    fill:none;
		    ...
	 */
	
	// list of layers minus 'g.'
	RS_StringList strListLayer = currentContent.split("g.", QString::SkipEmptyParts);
	
	// check if there is any g. information or stop this action
	int layerCount = strListLayer.count();
	if (layerCount == 0) return false;
	
	// loop through g. information
	for (int i = 0; i < layerCount; ++i) {
		// check for valid layer information
		if (strListLayer.at(i).startsWith("LAYER-")) {
			RS_String strLayerInfo = strListLayer.at(i);
			RS_String layerName = "";
			
			// extract layer name
			int index = strLayerInfo.indexOf("{");
			layerName = strLayerInfo;
			layerName.remove(index, strLayerInfo.size());
			layerName.remove("LAYER-");
			layerName = layerName.simplified(); // trimmed() only removes start end /t /f /r etc.
			
			// extract styles for layer
			RS_String strStyleInfo = strLayerInfo;
			strStyleInfo.remove(0, index);
			strStyleInfo.remove("}");
			strStyleInfo.simplified();
			RS_StringList strListStyle = strStyleInfo.split(";", QString::SkipEmptyParts);
			
			int styleCount = strListStyle.count();
			RS_Color color = RS_Color(255, 255, 255); // white default only
			RS2::LineWidth width = RS2::Width07; // 0.25 mm default only
			RS2::LineType type = RS2::SolidLine; 
			
			// loop through styles
			for (int j = 0; j < styleCount; ++j) {
				RS_String style = strListStyle.at(j);
				style = style.simplified();
				
				if (style.startsWith("stroke:")) {
					style.remove("stroke:rgb(");
					style.remove(")");
					RS_StringList strListColor = style.split(",", QString::SkipEmptyParts);
					
					if (strListColor.size() == 3) {
						int red = strListColor.at(0).toInt();
						int green = strListColor.at(1).toInt();
						int blue = strListColor.at(2).toInt();
						color.setRed(red);
						color.setGreen(green);
						color.setBlue(blue);
					}
				}
				
				if (style.startsWith("stroke-width:")) {
					style.remove("stroke-width:");
					double dblWidth = style.toDouble();
					width = RS2::intToLineWidth(int(dblWidth * 100));
				}

				if (style.startsWith("stroke-dasharray:")) {
					style.remove("stroke-dasharray:");
					
					// find out the relevant dot dash line
					
					
					
					double dblWidth = style.toDouble();
					width = RS2::intToLineWidth(int(dblWidth * 100));
				}
				
			}
			
			// set pen and create layer
			if (layerName != "0") {
				RS_Pen pen(color, width, type);
				RS_Layer* layer = new RS_Layer(RS_LayerData(layerName, pen, false, false)); 
				graphic->addLayer(layer);
			}
		}
	}
	
	CDATAsection = false;
	return true;
}

bool RB_FilterSvg::startElement(const QString& namespaceURI, const QString& localName,
                			const QString& qName, const QXmlAttributes& attribs) {
std::cout<<"RB_FilterSVG::startElement() "<< qName.toStdString().c_str() <<std::endl;
	return true;
}

bool RB_FilterSvg::endElement(const QString& namespaceURI, const QString& localName,
                			const QString& qName) {
	return true;
}

bool RB_FilterSvg::characters(const QString& str) {
    currentContent = str;
	return true;
}

bool RB_FilterSvg::fatalError(const QXmlParseException& exception) {
    // to std::out
    qWarning("Item %d, column %d: %s", exception.lineNumber(),
             exception.columnNumber(), exception.message().toStdString().c_str());
    
    // to QMessageBox
    this->parseMessage = "Item " + QString::number(exception.lineNumber()) +
    					" column " + QString::number(exception.columnNumber()) + 
    					": " + exception.message();
    return false;
}

QString RB_FilterSvg::getParseMessage() {
	return parseMessage;	
}

/** 
 * QCad export, as per rs_filterinterface
 * @param g QCad graphic container
 * @param file name of file to be exported
 * @param type file type to be saved, this filter is only for SVG file type
 * @return success 
 */
bool RB_FilterSvg::fileExport(RS_Graphic& g, const RS_String& file, 
										RS2::FormatType type) {
	if (type != RS2::FormatSvg) return false;
	
	// set graphic also here otherwise papersize will not work
	this->graphic = &g;
	return fileExport(g, file, QSize((int)PaperSize().x, (int)PaperSize().y), 
						false, false, false, false); 
}
    

/**
 * Extended export filter for RS_Graphic to SVG. This filter will browse through
 * the RS_Graphic as required to build a SVG file. The relevant data will be
 * passed to the RB_XmlSvg class where the data will be translated into SVG type
 * of XML. The RB_XmlSvg class will in turn call the generic RB_XmlWriter class
 * that writes the actual SVG(XML) file.
 * @param g The graphic (RS_Graphic) to be exported to SVG
 * @param file Full path to the SVG file that will be written.
 * @param format Format of exported file (in this case SVG)
 * @param size Pixel size of exported file
 * @param black Background to be set black
 * @param bw Export in black and white
 */
bool RB_FilterSvg::fileExport(RS_Graphic& g, const QString& file,
										const QSize& size, bool black, bool bw, 
										bool removeLayers,
										bool explodeBlocks) {
    
    RS_DEBUG->print("RB_FilterSVG::fileExport: exporting file '%s'...", 
   									(const char*)QFile::encodeName(file));
    this->graphic = &g;

    // check if we can write to that directory:
#ifndef Q_OS_WIN
    RS_String path = RS_FileInfo(file).absolutePath();
    if (RS_FileInfo(path).isWritable() == false) {
        RS_DEBUG->print("RB_FilterSVG::fileExport: can't write file: "
                        "no permission");
        return false;
    }
#endif

    // set version for SVG filter for later versions of SVG:
    RB_XmlCodes::TypeML exportType = RB_XmlCodes::TypeSVG;

    // Create RB_XmlWriter with the file (=name) and the exportType SVG
	// Note: svg is of class RB_XmlSvg, this will also write <?xml ... > 
    RB_XmlWriter* xw = svg.out((const char*)QFile::encodeName(file), exportType);

    if (xw == NULL) {
        RS_DEBUG->print("RB_FilterSVG::fileExport(): can't write file");
        return false;
    }

	// Set the unit of measurement, paper dimensions, coordinates, etc. 
	// in this rb_filtersvg object
	// Note: some of these variables are necessary elsewhere
	svg.setSize(size);
	svg.setBackgroundBlack(black);
	svg.setBlackWhite(bw);
	svg.setRemoveLayers(removeLayers);
	svg.setExplodeBlocks(explodeBlocks);
	svg.setPaperSize(PaperSize());
	svg.setPaperInsertionBase(PaperInsertionBase());

    // Write header <?xml ... ?> and <!DOCTYPE ... >
    svg.writeHeader(*xw);
    
    // Write Appid in comment
    writeAppid(*xw, RS_SYSTEM->getAppName() + " " + RS_SYSTEM->getAppVersion());

    // Write root tag <svg xmlns="http://www.w3.org/2000/svg" width="" ...>
    svg.writeRoot(*xw);

    // Write layer and block styles (styles in SVG)
	// note: both layers and block can contain styles
    writeStyles(*xw);	
	
	//
    // Write all block definitions, also the nested blocks in blocks,
    // here <g> in SVG <defs> element, the <symbol> gave problems with scaling.
	// Note: the 'insert' in QCad is the 'use' element in SVG
	//
    RS_DEBUG->print("RB_FilterSVG::fileExport() writing block definitions <g>");
    svg.writeDefinition(*xw);
    svg.setIsPartOfBlock(true);
    // symbol for arrowhead of leader and dimension
    svg.writeMarker(*xw); 
    
    for (uint i=0; i<(uint)graphic->countBlocks(); ++i) {
        RS_Block* blk = graphic->blockAt(i);
        // Save block if it's not a model or paper space:
        // Careful: other blocks with * / $ exist
        // TODO: check required or not
        //if (blk->getName().at(0)!='*' &&
        //        blk->getName().at(0)!='$') {
        writeBlock(*xw, blk);
        //}
    }
    
    svg.setIsPartOfBlock(false);
    svg.closeElement(*xw);  // close definitions </defs>
    
    //
    //  Write all layer entities (elements in SVG)
    //
    RS_DEBUG->print("RB_FilterSVG::fileExport() writing all entities per layer");

    for (uint i=0; i<(uint)graphic->countLayers(); ++i) {    
    	
        // l is string name(layer), pen, bool frozen, bool locked
		RS_Layer* l = graphic->layerAt(i);		
        writeLayer(*xw, l);

	    // RS_DEBUG->print("writing test element...");
	    // svg.writeTest(*xw);
	
	    RS_DEBUG->print("RB_FilterSVG::fileExport() writing layer entities START");

	    for (RS_Entity* e=graphic->firstEntity(RS2::ResolveNone);
	            e!=NULL;
	            e=graphic->nextEntity(RS2::ResolveNone)) {
	            if (l->getName() == e->getLayer()->getName() ) {
	                writeEntity(*xw, e);
	            }
	    }

	    RS_DEBUG->print("RB_FilterSVG::fileExport() writing layer entities OK");

		/*
		TODO: section OBJECTS
		(good example how to browse through entities and blocks)
	
	    if (type==RS2::FormatSVG) {
	        RS_DEBUG->print("writing section OBJECTS...");
	        svg.writeObjects(*xw);
	
	        // IMAGEDEF's from images in blocks
	        RS_StringList written;
	        for (uint i=0; i<graphic->countBlocks(); ++i) {
	            RS_Block* block = graphic->blockAt(i);
	            for (RS_Entity* e=block->firstEntity(RS2::ResolveAll);
	                    e!=NULL;
	                    e=block->nextEntity(RS2::ResolveAll)) {
	
	                if (e->rtti()==RS2::EntityImage) {
	                    RS_Image* img = ((RS_Image*)e);
	                    if (written.contains(file)==0 && img->getHandle()!=0) {
	                        writeImageDef(*xw, img);
	                        written.append(img->getFile());
	                    }
	                }
	            }
	        }
			// IMAGEDEF's from images in entities
	        for (RS_Entity* e=graphic->firstEntity(RS2::ResolveNone);
	                e!=NULL;
	                e=graphic->nextEntity(RS2::ResolveNone)) {
	
	            if (e->rtti()==RS2::EntityImage) {
	                RS_Image* img = ((RS_Image*)e);
	                if (written.contains(file)==0 && img->getHandle()!=0) {
	                    writeImageDef(*xw, img);
	                    written.append(img->getFile());
	                }
	            }
	        }
	        RS_DEBUG->print("writing end of section OBJECTS...");
	        svg.writeObjectsEnd(*xw);
	    }
	
	    RS_DEBUG->print("writing EOF...");
	    xw->svgEOF();
		*/


        // close layer (group) element </g>
        RS_DEBUG->print("writing close element </g>...");
        svg.closeElement(*xw);
    }

	// close root element </svg>
	RS_DEBUG->print("writing close element </svg>...");
	svg.closeElement(*xw);

    RS_DEBUG->print("close..");
    xw->close();

    delete xw;

    // check if file was actually written (strange world of windoze xp):
    if (RS_FileInfo(file).exists()==false) {
        RS_DEBUG->print("RB_FilterSVG::fileExport: file could not be written");
        return false;
    }
	
	return true;
}


/**
 * Writes all layer and block styles in <![CDATA[....]]> part of the SVG file.
 */
void RB_FilterSvg::writeStyles(RB_XmlWriter& xw) {
    RS_DEBUG->print("RB_FilterSVG::writeStyles()");
    
    // reset to clear all previous settings
    svgStyle.reset();

	// layer styles
    for (uint i=0; i<(uint)graphic->countLayers(); ++i) {
		// name(layer), pen, bool frozen, bool locked
		// rs_pen: lineType, lineWidth, double screenWidth, color 
		RS_Layer* l = graphic->layerAt(i);		

        if (l==NULL) {
            std::cerr << "RB_FilterSVG::writeLayer: Layer is NULL\n";
            return;
        }

        svgStyle.name = "LAYER-";
        svgStyle.name += l->getName().toLocal8Bit();

        if (l->isFrozen()) {
	        svgStyle.display = "none";
	    }
        
        setStyle(l->getPen());
    }
    
    svg.writeStyle(xw);
}


/**
 * Set style attributes for layer or block styles used in <![CDATA[....]]>
 * @param attrib svg attribute
 */
void RB_FilterSvg::setStyle(RS_Pen pen) {
    RS_DEBUG->print("RB_FilterSVG::setStyle ...");

	// already set in writeStyles()
    // svgSt.name = "layer-";
    // svgSt.name += l->getName().toLocal8Bit();
    // svgSt.display = "none" or nothing
    
    svgStyle.opacity = "1.0";
    svgStyle.fill = "none";
    
	// handle black-white and black background
	bool bkgrBlack = svg.isBackgroundBlack();
	bool bw = svg.isBlackWhite();
    
	if (!bkgrBlack && bw) {
    	svgStyle.stroke = "rgb(0,0,0)";
    } else if (bkgrBlack && bw) {
    	svgStyle.stroke = "rgb(255,255,255)";
	} else if (!bkgrBlack && !bw && RS_Color(255,255,255) == pen.getColor()) {
		svgStyle.stroke = "rgb(0,0,0)";
    } else { // bkgrBlack && !bw || !bkgrBlack && !bw
    	svgStyle.stroke = "rgb(" + RS_String::number(pen.getColor().red()) 
    					+ "," + RS_String::number(pen.getColor().green()) 
    					+ "," + RS_String::number(pen.getColor().blue()) 
    					+ ")";
    }
    
    svgStyle.strokeOpacity = "1.0";
    svgStyle.strokeWidth = RS_String::number(
    						(double)widthToNumber(pen.getWidth())/100);
    svgStyle.strokeLineCap = "round";
    svgStyle.strokeLineJoin = "round"; 

    RB_CadAttributes attr;    // for conversion to stroke-dasharray string
    attr.setLineType(pen.getLineType());
    if (pen.getLineType() == RS2::SolidLine){
        svgStyle.dashArray = "none";
    } else {
        svgStyle.dashArray = attr.getSvgLineType();
    }
    
    svgStyle.fontFamily = "helvetica";
    svgStyle.fontStretch = "extra-condensed";
    
    /* 
    layer can be expanded with following as RB_XmlCodes contains more SVG styles
    
    svgStyle.fontStyle;
    svgStyle.fontWeight;
    svgStyle.fontSize;
    svgStyle.textAnchor;
    */
    
    svg.setStyles(this->svgStyle);
}

/**
 * Writes a layer group <g>, the open element only, with class and id
 */
void RB_FilterSvg::writeLayer(RB_XmlWriter& xw, RS_Layer* l) {
    RS_DEBUG->print("RB_FilterSVG::writeLayer..."); 
    // RS_DEBUG->print((const char*)l->getName().toLocal8Bit());
    
    if (l==NULL) {
        std::cerr << "RB_FilterSVG::writeLayer: Layer is NULL\n";
        return;
    }

    svg.writeLayer(xw, DL_LayerData(string(l->getName().toStdString()),
    								(int)l->isFrozen()) );
}


/**
 * Writes an application id to the SVG file.
 * @param appid Application ID (e.g. "QCad").
 */
void RB_FilterSvg::writeAppid(RB_XmlWriter& xw, const RS_String& appid) {
    svg.writeAppid(xw, appid);
}


/**
 * Writes a block (just the definition, not the entities in it).
 */
void RB_FilterSvg::writeBlock(RB_XmlWriter& xw, RS_Block* blk) {
    if (blk==NULL) {
        std::cerr << "RB_FilterSVG::writeBlock(): Block is NULL\n";
        return;
    }
    
    RS_DEBUG->print("writing block: %s", (const char*)blk->getName().toLocal8Bit());

    if ("" == blk->getName().toLocal8Bit()) {
        std::cerr << "XML_Svg::writeBlock: Block name must not be empty\n";
        return;
    }
    
    // in QCad block can have attributes (color, width, linetype) 
    RB_CadAttributes attr = getEntityAttributes(blk);
    
    // calculate size of block, copy of the function PaperSize. todo: combine
    // double border = RS_Units::convert(25.0, RS2::Millimeter, graphic->getUnit());
    RS_Vector bSize = blk->getSize();

    // calculate the insertion point, copy of InsertionBase. todo: combine
	RS_Vector bInsBase = RS_Vector( (blk->getMin().x),(blk->getMin().y) );
    
    // open element e.g. <g id="name">
    svg.writeGroup(xw, 
                    DL_BlockData((const char*)blk->getName().toLocal8Bit(), 
	                                (int)blk->isFrozen(),
	                                blk->getBasePoint().x,
	                                blk->getBasePoint().y,
	                                blk->getBasePoint().z),
					attr);
    
    // write block elements divided in layer entities (elements in SVG)
    for (uint i=0; i<(uint)graphic->countLayers(); ++i) {    
    	
        int counter = 0;
		RS_Layer* l = graphic->layerAt(i);		
                
        // iterate through entities
        for (RS_Entity* e=blk->firstEntity(RS2::ResolveNone);
                    e!=NULL;
                    e=blk->nextEntity(RS2::ResolveNone)) {
            
            if (l->getName() == e->getLayer()->getName()) {
                if (0 == counter) {
                    svg.writeGroupLayerId(xw, 
                            DL_LayerData(string(l->getName().toStdString()),
                            (int)l->isFrozen()));
                }
                writeEntity(xw, e);
                ++counter;
            }
        
        }
        
        if (0 < counter) {
            svg.closeElement(xw); // close element </g>
        }
        
    }
    
    // entities in symbol are wrapped by an internal g element
    svg.closeElement(xw);  // close symbol </g>
}




/**
 * Writes the given entity to the SVG file.
 */
void RB_FilterSvg::writeEntity(RB_XmlWriter& xw, RS_Entity* e) {
    writeEntitySvg(xw, e, getEntityAttributes(e));
}


/**
 * Writes the given entity via the SVG filter to the XML writer.
 */
void RB_FilterSvg::writeEntitySvg(RB_XmlWriter& xw, RS_Entity* e, 
											RB_CadAttributes attrib) {

    if (e==NULL || e->getFlag(RS2::FlagUndone)) {
        return;
    }
    RS_DEBUG->print("writing Entity");

    switch (e->rtti()) {
	    case RS2::EntityPoint:
	        writePoint(xw, dynamic_cast<RS_Point*>(e), attrib);
	        break;
	    case RS2::EntityLine:
	        writeLine(xw, dynamic_cast<RS_Line*>(e), attrib);
	        break;
	    case RS2::EntityPolyline:
	        writePolyline(xw, dynamic_cast<RS_Polyline*>(e), attrib);
	        break;
	    case RS2::EntityVertex:
	        break;
	    case RS2::EntityCircle:
	        writeCircle(xw, dynamic_cast<RS_Circle*>(e), attrib);
	        break;
	    case RS2::EntityArc:
	        writeArc(xw, dynamic_cast<RS_Arc*>(e), attrib);
	        break;
	    case RS2::EntityEllipse:
	        writeEllipse(xw, dynamic_cast<RS_Ellipse*>(e), attrib);
	        break;
	    case RS2::EntityInsert:
	        writeInsert(xw, dynamic_cast<RS_Insert*>(e), attrib);
	        break;
	    case RS2::EntityText:
	        writeText(xw, dynamic_cast<RS_Text*>(e), attrib);
	        break;
	    case RS2::EntityDimAligned:
	    case RS2::EntityDimAngular:
	    case RS2::EntityDimLinear:
	    case RS2::EntityDimRadial:
	    case RS2::EntityDimDiametric:
	        writeDimension(xw, dynamic_cast<RS_Dimension*>(e), attrib);
	        break;
	    case RS2::EntityDimLeader:
	        writeLeader(xw, dynamic_cast<RS_Leader*>(e), attrib);
	        break;
	    case RS2::EntityHatch:
	//        writeHatch(xw, (RS_Hatch*)e, attrib);
	        break;
	    case RS2::EntityImage:
	//        writeImage(xw, (RS_Image*)e, attrib);
	        break;
	
	// #ifndef RS_NO_COMPLEX_ENTITIES
        case RS2::EntityBlock:
//        	// Blocks are only written in <defs> part of SVG file
//        	writeBlock(xw, (RS_Block*)e);
        	break;
	    case RS2::EntityContainer:
	//        writeEntityContainer(xw, (RS_EntityContainer*)e, attrib);
	        break;
	// #endif
	
	    default:
	        break;
    }
}


/**
 * Write point entity to the XML writer
 */
void RB_FilterSvg::writePoint(RB_XmlWriter& xw, RS_Point* p,
                              const RB_CadAttributes& attrib) {
    svg.writePoint(
        xw,
        DL_PointData(p->getPos().x,
                     p->getPos().y,
                     0.0),
        attrib);
}


/**
 * Write line to XML writer
 */
void RB_FilterSvg::writeLine(RB_XmlWriter& xw, RS_Line* l,
                             const RB_CadAttributes& attrib) {
    // RS_DEBUG->print("writing line element...");
	svg.writeLine(
        xw,
        DL_LineData(l->getStartpoint().x,
                    l->getStartpoint().y,
                    0.0,
                    l->getEndpoint().x,
                    l->getEndpoint().y,
                    0.0),
        attrib);
}


/**
 * Write polyline to XML writer
 */
void RB_FilterSvg::writePolyline(RB_XmlWriter& xw,
                                 RS_Polyline* pl,
                                 const RB_CadAttributes& attrib) {
    bool first = true;

    for (RS_Entity* v=pl->firstEntity(RS2::ResolveNone);
            v!=NULL;
            v=pl->nextEntity(RS2::ResolveNone)) {

        // Write line vertices:
        if (v->rtti()==RS2::EntityLine) {
            RS_Line* l = dynamic_cast<RS_Line*>(v);
            if (first) {
                svg.writeVertex(DL_VertexData(l->getStartpoint().x,
                                              l->getStartpoint().y,
                                              0.0, first));
                first = false;
            }
            // todo: do not write last vertex if polyline is closed 
            // but use 'z' in xml_svg.cpp. Does not work yet
            // if ( !(pl->isClosed()) && !(v == (pl->lastEntity(RS2::ResolveNone))) ) {

                svg.writeVertex(DL_VertexData(l->getEndpoint().x,
                                              l->getEndpoint().y,
                                              0.0, first));
            // }
        }

        // Write arc vertices:
        else if (v->rtti()==RS2::EntityArc) {
            RS_Arc* a = dynamic_cast<RS_Arc*>(v);
            if (first) {
                svg.writeVertex(DL_VertexData(a->getStartpoint().x,
                                              a->getStartpoint().y,
                                              0.0, first));
                first = false;
            }
            svg.writeVertex(DL_VertexData(a->getEndpoint().x,
                                          a->getEndpoint().y,
                                          a->getBulge(), first));
        }
    }

    svg.writePolyline(
        xw,
        DL_PolylineData(pl->count()+1,
                        0, 0,
                        pl->isClosed()*0x1),
                        attrib);
}


/**
 * Write circle to XML writer
 */
void RB_FilterSvg::writeCircle(RB_XmlWriter& xw, RS_Circle* c,
                               const RB_CadAttributes& attrib) {
    svg.writeCircle(
        xw,
        DL_CircleData(c->getCenter().x,
                      c->getCenter().y,
                      0.0,
                      c->getRadius()),
        attrib);

}


/**
 * Write arc to XML writer
 */
void RB_FilterSvg::writeArc(RB_XmlWriter& xw, RS_Arc* a,
                            const RB_CadAttributes& attrib) {
    svg.writeArc(
        xw,
        DL_ArcData(a->getCenter().x,
                   a->getCenter().y,
                   0.0,
                   a->getRadius(),
                   a->getAngle1(),
                   a->getAngle2()),
        attrib);
}


/**
 * Write ellipse to XML writer
 */
void RB_FilterSvg::writeEllipse(RB_XmlWriter& xw, RS_Ellipse* s,
                                const RB_CadAttributes& attrib) {
    svg.writeEllipse(
        xw,
        DL_EllipseData(s->getCenter().x,
                       s->getCenter().y,
                       0.0,
                       s->getMajorP().x,
                       s->getMajorP().y,
                       0.0,
                       s->getRatio(),
                       s->getAngle1(),
                       s->getAngle2()),
        attrib);
}


/**
 * Write insert (the location of a block) to the XML writer
 */
void RB_FilterSvg::writeInsert(RB_XmlWriter& xw, RS_Insert* i,
                               const RB_CadAttributes& attrib) {
    RS_DEBUG->print("RB_FilterSVG::writeInsert()"); 

    RS_Block* blk = i->getBlockForInsert();
    // sometimes inserts do not have a block (NULL) in QCad
    if (!blk) return;
    
    // calculate size of block, copy of the function PaperSize. todo: combine
    double border = RS_Units::convert(25.0, RS2::Millimeter, graphic->getUnit());
    RS_Vector bSize = blk->getSize() + RS_Vector(border, border);

    // calculate the insertion point, copy of InsertionBase. todo: combine
	RS_Vector bInsBase = RS_Vector((blk->getMin().x - border/2),
									-(blk->getMin().y - border/2));

    svg.writeUse(
        xw,
        DL_InsertData(string(i->getName().toStdString()),
                      i->getInsertionPoint().x,
                      i->getInsertionPoint().y,
                      0.0,
                      i->getScale().x,
                      i->getScale().y,
                      0.0,
                      i->getAngle()*ARAD,
                      i->getCols(), i->getRows(),
                      i->getSpacing().x,
                      i->getSpacing().y),
        DL_BlockData(string(blk->getName().toStdString()), 0,
                      blk->getBasePoint().x,
                      blk->getBasePoint().y,
                      blk->getBasePoint().z),
        attrib);
}


/**
 * Write text to XML writer
 */
void RB_FilterSvg::writeText(RB_XmlWriter& xw, RS_Text* t,
                             	RB_CadAttributes& attrib) {
        int attachmentPoint=1;
        if (t->getHAlign()==RS2::HAlignLeft) {
            attachmentPoint=1;
        } else if (t->getHAlign()==RS2::HAlignCenter) {
            attachmentPoint=2;
        } else if (t->getHAlign()==RS2::HAlignRight) {
            attachmentPoint=3;
        }
        if (t->getVAlign()==RS2::VAlignTop) {
            attachmentPoint+=0;
        } else if (t->getVAlign()==RS2::VAlignMiddle) {
            attachmentPoint+=3;
        } else if (t->getVAlign()==RS2::VAlignBottom) {
            attachmentPoint+=6;
        }
/* Handled by RB_XmlSvg svg.writeMText()
		// get text fill color from layer or block
	    if (attrib.getColor().isByLayer()) {
			RS_Layer* l = graphic->findLayer(attrib.getLayer());
			attrib.setColor(l->getPen().getColor());	    	
	    } else if (attrib.getColor().isByBlock()) { 
    	    RS_Block* b = graphic->findBlock(attrib.getBlock());
			attrib.setColor(b->getPen().getColor());	    	
    	} else {
			attrib.setColor(attrib.getColor());	    	
    	}

		// handle black-white and black background
		bool bkgrBlack = svg.isBackgroundBlack();
		bool bw = svg.isBlackWhite();
        
		if (!bkgrBlack && bw) {
			attrib.setStrokeColor(RS_Color(0,0,0));
        } else if (bkgrBlack && bw) {
			attrib.setStrokeColor(RS_Color(255,255,255));
        } // else leave fill color as is
*/
        svg.writeMText(
            xw,
            DL_MTextData(t->getInsertionPoint().x,
                         t->getInsertionPoint().y,
                         0.0,
                         t->getHeight(),
                         t->getWidth(),
                         attachmentPoint,
                         t->getDrawingDirection(),
                         t->getLineSpacingStyle(),
                         t->getLineSpacingFactor(),
                         (const char*)toSvgString(
                             t->getText()).toLocal8Bit(),
                         (const char*)t->getStyle().toLocal8Bit(),
                         t->getAngle()),
            attrib);
}


/**
 * Write dimension to XML writer
 */
void RB_FilterSvg::writeDimension(RB_XmlWriter& xw, RS_Dimension* d,
                                  const RB_CadAttributes& attrib) {
    int type;
    int attachmentPoint=1;

    if (d->getHAlign()==RS2::HAlignLeft) {
        attachmentPoint=1;
    } else if (d->getHAlign()==RS2::HAlignCenter) {
        attachmentPoint=2;
    } else if (d->getHAlign()==RS2::HAlignRight) {
        attachmentPoint=3;
    }

    if (d->getVAlign()==RS2::VAlignTop) {
        attachmentPoint+=0;
    } else if (d->getVAlign()==RS2::VAlignMiddle) {
        attachmentPoint+=3;
    } else if (d->getVAlign()==RS2::VAlignBottom) {
        attachmentPoint+=6;
    }

    switch (d->rtti()) {
	    case RS2::EntityDimAligned:
	        type = 1;
	        break;
	    case RS2::EntityDimLinear:
	        type = 0;
	        break;
	    case RS2::EntityDimRadial:
	        type = 4;
	        break;
	    case RS2::EntityDimDiametric:
	        type = 3;
	        break;
	    default:
	        type = 0;
	        break;
    }
    
    DL_DimensionData dimData(d->getDefinitionPoint().x,
                             d->getDefinitionPoint().y,
                             0.0,
                             d->getMiddleOfText().x,
                             d->getMiddleOfText().y,
                             0.0,
                             type,
                             attachmentPoint,
                             d->getLineSpacingStyle(),
                             d->getLineSpacingFactor(),
                             (const char*)toSvgString(
                                 d->getText()).toLocal8Bit(),
                             (const char*)d->getStyle().toLocal8Bit(),
                             d->getAngle());
                             

    if (d->rtti()==RS2::EntityDimAligned) {
        RS_DimAligned* da = (RS_DimAligned*)d;

        DL_DimAlignedData dimAlignedData(da->getExtensionPoint1().x,
                                         da->getExtensionPoint1().y,
                                         0.0,
                                         da->getExtensionPoint2().x,
                                         da->getExtensionPoint2().y,
                                         0.0);

        svg.writeDimAligned(xw, dimData, dimAlignedData, attrib);
    } else if (d->rtti()==RS2::EntityDimLinear) {
        RS_DimLinear* dl = (RS_DimLinear*)d;

        DL_DimLinearData dimLinearData(dl->getExtensionPoint1().x,
                                       dl->getExtensionPoint1().y,
                                       0.0,
                                       dl->getExtensionPoint2().x,
                                       dl->getExtensionPoint2().y,
                                       0.0,
                                       dl->getAngle(),
                                       dl->getOblique());

        svg.writeDimLinear(xw, dimData, dimLinearData, attrib);
    } else if (d->rtti()==RS2::EntityDimRadial) {
        RS_DimRadial* dr = (RS_DimRadial*)d;

        DL_DimRadialData dimRadialData(dr->getDefinitionPoint().x,
                                       dr->getDefinitionPoint().y,
                                       0.0,
                                       dr->getLeader());

        svg.writeDimRadial(xw, dimData, dimRadialData, attrib);
    } else if (d->rtti()==RS2::EntityDimDiametric) {
        RS_DimDiametric* dr = (RS_DimDiametric*)d;

        DL_DimDiametricData dimDiametricData(dr->getDefinitionPoint().x,
                                             dr->getDefinitionPoint().y,
                                             0.0,
                                             dr->getLeader());

        svg.writeDimDiametric(xw, dimData, dimDiametricData, attrib);
    } else if (d->rtti()==RS2::EntityDimAngular) {
        RS_DimAngular* da = (RS_DimAngular*)d;

        DL_DimAngularData dimAngularData(da->getDefinitionPoint1().x,
                                         da->getDefinitionPoint1().y,
                                         0.0,
                                         da->getDefinitionPoint2().x,
                                         da->getDefinitionPoint2().y,
                                         0.0,
                                         da->getDefinitionPoint3().x,
                                         da->getDefinitionPoint3().y,
                                         0.0,
                                         da->getDefinitionPoint4().x,
                                         da->getDefinitionPoint4().y,
                                         0.0);

        svg.writeDimAngular(xw, dimData, dimAngularData, attrib);
    }

}


/**
 * Write leader to XML writer
 */
void RB_FilterSvg::writeLeader(RB_XmlWriter& xw, RS_Leader* l,
                               const RB_CadAttributes& attrib) {
    
    if (l->count() < 1) {
    	std::cerr << "dropping leader with no vertices\n";
    }
        
    bool first = true;
    
    for (RS_Entity* v=l->firstEntity(RS2::ResolveNone);
            v!=NULL;
            v=l->nextEntity(RS2::ResolveNone)) {

        // Write line vertices in path string:
        if (v->rtti()==RS2::EntityLine) {
            RS_Line* l = dynamic_cast<RS_Line*>(v);

            if (first) {
                svg.writeLeaderVertex(
                    xw,
                    DL_LeaderVertexData(l->getStartpoint().x,
                                        l->getStartpoint().y,
                                        0.0)); 
                first = false;
            }

            svg.writeLeaderVertex(
                xw,
                DL_LeaderVertexData(l->getEndpoint().x,
                                    l->getEndpoint().y,
                                    0.0));
        }
    }

    svg.writeLeader(
        xw,
        DL_LeaderData(l->hasArrowHead(),
                      0,
                      3,
                      0,
                      0,
                      1.0,
                      10.0,
                      l->count()),
        attrib);
}


/**
 * Get the entity attributes as a RB_CadAttributes object.
 * 
 * @retval attrib XML attributes: layer name, color, line width, line type
 */
RB_CadAttributes RB_FilterSvg::getEntityAttributes(RS_Entity* entity) {

    // layer
    RS_String layerName = "";
    RS_Layer* layer = entity->getLayer();

    if (layer!=NULL) {
        layerName = layer->getName();
    }

    // block
    RS_String blockName = "";
    RS_Block* block = entity->getBlock();

    if (block!=NULL) {
        blockName = block->getName();
    }

    // Relevant pen object
    RS_Pen pen;
    
    if (entity->rtti() == RS2::EntityText) {
    	// resolve color, lineWidth and lineType to actual type
    	pen = entity->getPen(true);
    } else {
    	// if not text keep at this entity, including 'BYLAYER' and 'BYBLOCK'
    	pen = entity->getPen(false);
    }

    // Color
    RS_Color color = pen.getColor();
    
    // Linetype
    RS2::LineType lineType = pen.getLineType();

    // Width, TODO: update conversion independend of millimeters
	int tempWidth = widthToNumber(pen.getWidth());
    double lineWidth = 0.0;

    if (tempWidth > 0) {
        lineWidth = (double)tempWidth / 100; // to get millimeter
    } else {
    	// minus means by layer (-1) or by block (-2)
        lineWidth = (double)tempWidth;
    }
    
    // compensate lineWidth for stroke used on bold text
    if (entity->rtti() == RS2::EntityText && lineWidth > 0.35) {
    	lineWidth -= 0.35; // add extra width for bold text
    } else if (entity->rtti() == RS2::EntityText) {
    	lineWidth = 0.0;
    }
    
    /* Create temporary attrib object
     * layername can be used directly in the svg code as attribute
     * color in red, green, blue component 
     *   (no difference in strokeColor and fillColor here)
     * lineWidth is now in mm
     * linetype needs to be transformed in a continuous line or a dasharray for svg
     * for block set color, line width and line type by layer, no attributes to be written 
     */
    if (entity->rtti() == RS2::EntityBlock) {
    	color.setFlags(RS2::FlagByLayer); // set flag by layer
    	lineWidth = -1.0;
    	lineType = RS2::LineByLayer;
    }
    RB_CadAttributes attrib(layerName, blockName, color, lineWidth, lineType); 

    /* from rs_pen
	RS2::LineType lineType;
    RS2::LineWidth width;
	double screenWidth;
    RS_Color color;
	*/
    
	return attrib;
}


/**
 * Converts a color into a color number in the SVG palette.
 * The color that fits best is chosen.
 */
int RB_FilterSvg::colorToNumber(const RS_Color& col) {

    // Special color BYBLOCK:
    if (col.getFlag(RS2::FlagByBlock)) {
        return 0;
    }

    // Special color BYLAYER
    else if (col.getFlag(RS2::FlagByLayer)) {
        return 256;
    }

    // Special color black is not in the table but white represents both
    // black and white
    else if (col.red()==0 && col.green()==0 && col.blue()==0) {
        return 7;
    }

    // All other colors
    else {
        int num=0;
        int diff=255*3;  // smallest difference to a color in the table found so far

        // Run through the whole table and compare
        for (int i=1; i<=255; i++) {
            int d = abs(col.red()-(int)(svgColors[i][0]*255))
                    + abs(col.green()-(int)(svgColors[i][1]*255))
                    + abs(col.blue()-(int)(svgColors[i][2]*255));

            if (d<diff) {
                /*
                printf("color %f,%f,%f is closer\n",
                       svgColors[i][0],
                       svgColors[i][1],
                       svgColors[i][2]);
                */
                diff = d;
                num = i;
                if (d==0) {
                    break;
                }
            }
        }
        //printf("  Found: %d, diff: %d\n", num, diff);
        return num;
    }
}


/**
 * Converts a RS_LineType into a name for a line type.
 * /
RS_String RB_FilterSvg::lineTypeToName(RS2::LineType lineType) {

    // Standard linetypes for QCad II / AutoCAD
    switch (lineType) {
	
	    case RS2::SolidLine:
	        return "CONTINUOUS";
	        break;
	
	    case RS2::DotLine:
	        return "DOT";
	        break;
	    case RS2::DotLine2:
	        return "DOT2";
	        break;
	    case RS2::DotLineX2:
	        return "DOTX2";
	        break;
	
	    case RS2::DashLine:
	        return "DASHED";
	        break;
	    case RS2::DashLine2:
	        return "DASHED2";
	        break;
	    case RS2::DashLineX2:
	        return "DASHEDX2";
	        break;
	
	    case RS2::DashDotLine:
	        return "DASHDOT";
	        break;
	    case RS2::DashDotLine2:
	        return "DASHDOT2";
	        break;
	    case RS2::DashDotLineX2:
	        return "DASHDOTX2";
	        break;
	
	    case RS2::DivideLine:
	        return "DIVIDE";
	        break;
	    case RS2::DivideLine2:
	        return "DIVIDE2";
	        break;
	    case RS2::DivideLineX2:
	        return "DIVIDEX2";
	        break;
	
	    case RS2::CenterLine:
	        return "CENTER";
	        break;
	    case RS2::CenterLine2:
	        return "CENTER2";
	        break;
	    case RS2::CenterLineX2:
	        return "CENTERX2";
	        break;
	
	    case RS2::BorderLine:
	        return "BORDER";
	        break;
	    case RS2::BorderLine2:
	        return "BORDER2";
	        break;
	    case RS2::BorderLineX2:
	        return "BORDERX2";
	        break;
	
	
	    case RS2::LineByLayer:
	        return "BYLAYER";
	        break;
	    case RS2::LineByBlock:
	        return "BYBLOCK";
	        break;
	    default:
	        break;
    }

    return "CONTINUOUS";
}
*/

/**
 * Converts a RS2::LineWidth into an int width.
 */
int RB_FilterSvg::widthToNumber(RS2::LineWidth width) {

    switch (width) {
	    case RS2::WidthByLayer:
	        return -1;
	        break;
	    case RS2::WidthByBlock:
	        return -2;
	        break;
	    case RS2::WidthDefault:
	        return -3;
	        break;
	    default:
	        return (int)width;
	        break;
    }

    return (int)width;
}



/**
 * Converts a native unicode string into a SVG encoded string.
 *
 * SVG encoding includes the following special sequences:
 * - %%%c for a diameter sign
 * - %%%d for a degree sign
 * - %%%p for a plus/minus sign
 */
RS_String RB_FilterSvg::toSvgString(const RS_String& string) {
    /*
       RS_String res = string;
       // Line feed:
       res = res.replace(RS_RegExp("\\n"), "\\P");
       // Space:
       res = res.replace(RS_RegExp(" "), "\\~");
       // diameter:
       res = res.replace(QChar(0x2205), "%%c");
       // degree:
       res = res.replace(QChar(0x00B0), "%%d");
       // plus/minus
       res = res.replace(QChar(0x00B1), "%%p");
    */

    RS_String res = "";

    for (uint i=0; i<(uint)string.length(); ++i) {
        int c = string.at(i).unicode();
        //std::cout << "toSvgString: " << c << "\n";
        switch (c) {
	        case 0x0A:
	            res+="\\P";
	            break;
	        case 0x20:
	            res+="\\~";
	            break;
	            // diameter:
	        case 0x2205:
	            res+="%%c";
	            break;
	            // degree:
	        case 0x00B0:
	            res+="%%d";
	            break;
	            // plus/minus
	        case 0x00B1:
	            res+="%%p";
	            break;
	        default:
	            if (c>127) {
	                RS_String hex;
	                hex = RS_String("%1").arg(c, 4, 16);
#if QT_VERSION>=0x030000
	                hex = hex.replace(' ', '0');
#else
    	            hex = RS_StringCompat::replace(hex, ' ', '0');
#endif
	                res+=RS_String("\\U+%1").arg(hex);
	            } else {
	                res+=string.at(i);
	            }
	            break;
        }
    }

    return res;
}


/**
 * Returns the SVG viewBox coordinates to fit the graphic in the viewport
 * An additional margin of 25.0/2 is added at each side of the paper size
 */
RS_Vector RB_FilterSvg::PaperSize() {
    double border = RS_Units::convert(25.0, RS2::Millimeter, graphic->getUnit());
    RS_Vector pSize = graphic->getSize() + RS_Vector(border, border);
    return pSize;
}


/**
 * Returns the SVG viewBox shift of origine coordinates to place the graphic 
 * in the viewport
 */
RS_Vector RB_FilterSvg::PaperInsertionBase() {
    double border = RS_Units::convert(25.0, RS2::Millimeter, graphic->getUnit());
	RS_Vector pInsBase = RS_Vector((graphic->getMin().x - border/2),
									-(graphic->getMin().y - border/2) );
	return pInsBase;
}

// EOF


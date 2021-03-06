/****************************************************************************
** $Id: rs_fileio.cpp 10815 2008-06-24 13:05:54Z andrew $
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

#include "rs_dir.h"
#include "rs_file.h"
#include "rs_fileinfo.h"
#include "rs_fileio.h"
#include "rs_textstream.h"


RS_FileIO* RS_FileIO::uniqueInstance = NULL;
    
    
RS_FileIO::RS_FileIO() {
    filterList.setAutoDelete(true);
}

/**
 * Registers a new import filter.
 */
void RS_FileIO::registerFilter(RS_FilterInterface* f) {
    filterList.append(f);
}

/**
 * @return List of registered filters.
 */
RS_PtrList<RS_FilterInterface> RS_FileIO::getFilterList() {
    return filterList;
}


/**
 * @return File dialog filter string for the given format type or "".
 */
RS_String RS_FileIO::getFilterString(RS2::FormatType t) {
    RS_String s;
    for (int i=0; i<filterList.size(); ++i) {
        s = filterList[i]->getFilterString(t);
        if (!s.isEmpty()) {
            return s;
        }
    }

    return "";
}


/**
 * @return List of filter strings of all available import filters.
 */
RS_StringList RS_FileIO::getAllImportFilterStrings() {
    RS_StringList ret;
    
    for (int i=0; i<filterList.size(); ++i) {
        for (RS2::FormatType t=RS2::FormatUnknown; t<RS2::FormatLast; t=(RS2::FormatType)((int)t+1)) {
            if (filterList[i]->canImport(t)) {
                RS_String filterString = filterList[i]->getFilterString(t);
                if (!filterString.isEmpty()) {
                    RS_DEBUG->print("RS_FileIO::getAllImportFilterStrings:"
                        "appending filter: %s", (const char*)filterString.toLatin1());
                    ret.append(filterString);
                }
            }
        }
    }

    return ret;
}



/**
 * @return List of filter strings of all available export filters.
 */
RS_StringList RS_FileIO::getAllExportFilterStrings() {
    RS_StringList ret;
    
    for (int i=0; i<filterList.size(); ++i) {
        for (RS2::FormatType t=RS2::FormatUnknown; t<RS2::FormatLast; t=(RS2::FormatType)((int)t+1)) {
            if (filterList[i]->canExport(t)) {
                RS_String filterString = filterList[i]->getFilterString(t);
                if (!filterString.isEmpty()) {
                    RS_DEBUG->print("RS_FileIO::getAllExportFilterStrings:"
                        "appending filter: %s", (const char*)filterString.toLatin1());
                    ret.append(filterString);
                }
                else {
                    RS_DEBUG->print(RS_Debug::D_WARNING,
                        "RS_FileIO::getAllExportFilterStrings: "
                            "filter claims to export format %d, "
                            "but does not return filter string", 
                            (int)t);
                }
            }
        }
    }

    return ret;
}

/**
 * @return The format type that corresponds to the given filter string.
 * The filter string might for example have been selected by the user 
 * in a file open dialog.
 */
RS2::FormatType RS_FileIO::getType(const RS_String& filterString) {
    for (int i=0; i<filterList.size(); ++i) {
        for (RS2::FormatType t=RS2::FormatUnknown; t<RS2::FormatLast; t=(RS2::FormatType)((int)t+1)) {
            if (filterList[i]->getFilterString(t)==filterString) {
                return t;
            }
        }
    }

    return RS2::FormatUnknown;
}

/**
 * @return Filter which can import the given file type.
 */
RS_FilterInterface* RS_FileIO::getImportFilter(RS2::FormatType t) {
    for (int i=0; i<filterList.size(); ++i) {
        if (filterList[i]->canImport(t)) {
            return filterList[i];
        }
    }

    return NULL;
}

/**
 * @return Filter which can export the given file type.
 */
RS_FilterInterface* RS_FileIO::getExportFilter(RS2::FormatType t) {
    for (int i=0; i<filterList.size(); ++i) {
        if (filterList[i]->canExport(t)) {
            return filterList[i];
        }
    }

    return NULL;
}


/**
 * Calls the import method of the filter responsible for the format
 * of the given file.
 *
 * @param graphic The container to which we will add
 *        entities. Usually that's an RS_Graphic entity but
 *        it can also be a polyline, text, ...
 * @param file Path and name of the file to import.
 */
bool RS_FileIO::fileImport(RS_Graphic& graphic, const RS_String& fileName, 
        RS2::FormatType type) {

    RS_DEBUG->print("RS_FileIO::fileImport: "
        "Trying to import file '%s'...", 
        (const char*)fileName.toLatin1());

    RS_FilterInterface* filter = NULL;

    RS2::FormatType t;
    if (type == RS2::FormatUnknown) {
        t = detectFormat(fileName);
    }
    else {
        t = type;
    }
    
    RS_DEBUG->print("RS_FileIO::fileImport: "
        "get import filter for type %d", (int)t);

    filter = getImportFilter(t);

    if (filter!=NULL) {
        // disable updates during loading:
        if (graphic.getLayerList()!=NULL) {
            graphic.getLayerList()->blockSignals(true);
        }
        
        bool ret = filter->fileImport(graphic, fileName, t);
        
        // enable updates and force update:
        if (graphic.getLayerList()!=NULL) {
            graphic.getLayerList()->blockSignals(false);
            graphic.getLayerList()->update();
        }

        return ret;
    }
    else {
        RS_DEBUG->print(RS_Debug::D_WARNING,
            "RS_FileIO::fileImport: failed to import file: %s. No filter found.", 
            (const char*)fileName.toLatin1());
    }
    
    return false;
}



/**
 * Calls the export method of the object responsible for the format
 * of the given file.
 *
 * @param file Path and name of the file to import.
 */
bool RS_FileIO::fileExport(RS_Graphic& graphic, const RS_String& fileName,
        RS2::FormatType type) {

    RS_DEBUG->print("RS_FileIO::fileExport");
    //RS_DEBUG->print("Trying to export file '%s'...", (const char*)fileName.toLatin1());

    if (type==RS2::FormatUnknown) {
        RS_String extension;
        extension = RS_FileInfo(fileName).suffix().toLower();

        type = detectFormat(fileName, false);
        /*
        if (extension=="dxf") {
            type = RS2::FormatDxf;
        }
        else if (extension=="cxf") {
            type = RS2::FormatCxf;
        }
        */
    }

    RS_FilterInterface* filter = getExportFilter(type);
    if (filter!=NULL) {
        bool ret = filter->fileExport(graphic, fileName, type);

        if (ret) {
            RS_FileInfo fi(fileName);
            RS_String path = fi.absolutePath();
            RS_String filePath = 
                RS_Dir::toNativeSeparators(path) + 
                RS_Dir::separator() + 
                fi.fileName();

            // stream absolute path of exported file to stdout.
            // can be used for simple interaction with the calling 
            // process
            std::cout 
                << (const char*)filePath.toLatin1() 
                << std::endl;
        }
        else {
            RS_DEBUG->print("RS_FileIO::fileExport: "
                "export filter returned false");
        }

        return ret;
    }
    
    RS_DEBUG->print(RS_Debug::D_WARNING, "RS_FileIO::fileExport: no filter found");

    return false;
}


/**
 * Detects and returns the file format of the given file name or extension.
 *
 * @param fileName Complete file name, optionally with path or only extension.
 * @param exists The file exists and can be used for detection.
 */
RS2::FormatType RS_FileIO::detectFormat(const RS_String& fileName, bool exists) {
    RS_DEBUG->print("RS_FileIO::detectFormat from '%s'", 
        (const char*)fileName.toLatin1());

    RS2::FormatType type = RS2::FormatUnknown;

    RS_String ext;
    if (fileName.contains('.')) {
        ext = RS_FileInfo(fileName).suffix().toLower();
    }
    else {
        ext = fileName.toLower();
    }
    
    RS_DEBUG->print("RS_FileIO::detectFormat ext: '%s'", 
        (const char*)ext.toLatin1());

    if (ext=="cxf") {
        type = RS2::FormatCxf;
    } else if (ext=="redf" || ext=="red") {
        type = RS2::FormatRedf;
    } else if (ext=="flt") {
        type = RS2::FormatFlt;
    } else if (ext=="dae") {
        type = RS2::FormatDae;
    } else if (ext=="nif") {
        type = RS2::FormatNif;
    } else if (ext=="3ds") {
        type = RS2::Format3ds;
    } else if (ext=="rwbe" || ext=="rwb") {
        type = RS2::FormatRwbe;
    } else if (ext=="rdxf" || ext=="rdx") {
        type = RS2::FormatDxfArch;
    } else if (ext=="svg") {
        type = RS2::FormatSvg;
    } else if (ext=="shp" || ext=="shx") {
        type = RS2::FormatShp;
    } else if (ext=="dxf") {
        if (!exists) {
            type = RS2::FormatDxf;
        }
        else {
            type = RS2::FormatDxf1;
            RS_File f(fileName);

            if (!f.open(QIODevice::ReadOnly)) {
                // Error opening file:
                RS_DEBUG->print(RS_Debug::D_WARNING,
                    "RS_FileIO::detectFormat: Cannot open file: %s", 
                    (const char*)fileName.toLatin1());
                type = RS2::FormatUnknown;
            } else {
                RS_DEBUG->print("RS_FileIO::detectFormat: "
                    "Successfully opened DXF file: %s",
                    (const char*)fileName.toLatin1());

                RS_TextStream ts(&f);
                RS_String line;
                int c=0;
                while (!ts.atEnd() && ++c<100) {
                    line = ts.readLine();
                    RS_DEBUG->print("RS_FileIO::detectFormat: line: %d: %s", 
                        c, (const char*)line.toLatin1());
                    if (line=="$ACADVER") {
                        type = RS2::FormatDxf;
                    }
                    // very simple reduced DXF:
                    if (line=="ENTITIES" && c<10) {
                        type = RS2::FormatDxf;
                    }
                }
                f.close();
            }
        }
    }
    
    RS_DEBUG->print("RS_FileIO::detectFormat: OK"); 

    return type;
}


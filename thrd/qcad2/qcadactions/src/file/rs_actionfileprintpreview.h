/****************************************************************************
** $Id: rs_actionfileprintpreview.h 10343 2008-04-06 17:43:02Z andrew $
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

#ifndef RS_ACTIONFILEPRINTPREVIEW_H
#define RS_ACTIONFILEPRINTPREVIEW_H

#include "rs_actioninterface.h"

/**
 * Default action for print preview.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_ActionFilePrintPreview : public RS_ActionInterface {
    Q_OBJECT
public:
    /**
     * Action States.
     */
    enum Status {
        Neutral,
        Moving
    };

public:
    RS_ActionFilePrintPreview(RS_EntityContainer& container,
                        RS_GraphicView& graphicView);
    ~RS_ActionFilePrintPreview();

    static RS_String getName() {
        return "File Print Preview";
    }

    virtual RS_String name() {
        return RS_ActionFilePrintPreview::getName();
    }

    static RS_GuiAction* createGuiAction();

    virtual void init(int status=0);

    virtual void trigger();

    virtual void mouseMoveEvent(RS_MouseEvent* e);
    virtual void mousePressEvent(RS_MouseEvent* e);
    virtual void mouseReleaseEvent(RS_MouseEvent* e);

    virtual void coordinateEvent(RS_CoordinateEvent* e);
    virtual void commandEvent(RS_CommandEvent* e);
    virtual RS_StringList getAvailableCommands();

    virtual void updateMouseButtonHints();
    virtual void updateMouseCursor();
    virtual void updateToolBar();

    void center();
    void fit();
    void setScale(double f);
    double getScale();

    void setBlackWhite(bool bw);
    //bool isBlackWhite() {
    //    return blackWhite;
    //}
    RS2::Unit getUnit();

protected:
    //bool blackWhite;
    RS_Vector v1;
    RS_Vector v2;
};

#endif

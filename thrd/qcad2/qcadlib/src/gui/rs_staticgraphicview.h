/*****************************************************************************
**  $Id: rs_staticgraphicview.h 3747 2006-12-03 04:18:06Z andrew $
**
**  This is part of the QCad Qt GUI
**  Copyright (C) 2001 Andrew Mustun
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License (version 2) as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

#ifndef RS_STATICGRAPHICVIEW_H
#define RS_STATICGRAPHICVIEW_H

#include "rs_graphicview.h"

/**
 * This is an implementation of a graphic viewer with a fixed size
 * for drawing onto fixed devices (such as bitmaps).
 */
class CAD_EXPORT RS_StaticGraphicView: public RS_GraphicView {
public:
    RS_StaticGraphicView(int w, int h, RS_Painter* p);
    virtual ~RS_StaticGraphicView();

    virtual int getWidth();
    virtual int getHeight();
    virtual void redraw() {}
    virtual void redrawPreview() {}
    virtual void restore() { }
    virtual void adjustOffsetControls() {}
    virtual void adjustZoomControls() {}
    virtual RS_Painter* createPainter();
    virtual RS_Painter* createDirectPainter();
    virtual RS_Painter* createSimPainter();
    virtual void destroyPainter();
    virtual void setMouseCursor(RS2::CursorType ) {}

    virtual void emulateMouseMoveEvent() {}
    virtual void emulateMousePressEvent() {}
    virtual void emulateMouseReleaseEvent() {}
    virtual void updateGridStatusWidget(const RS_String& ) {}

    void paint();

private:
    //! Width
    int width;

    //! Height
    int height;
};

#endif

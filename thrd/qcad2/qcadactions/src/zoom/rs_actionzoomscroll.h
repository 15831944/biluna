/****************************************************************************
** $Id: rs_actionzoomscroll.h 10343 2008-04-06 17:43:02Z andrew $
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

#ifndef RS_ACTIONZOOMSCROLL_H
#define RS_ACTIONZOOMSCROLL_H

#include "rs_actioninterface.h"


/**
 * This action triggers a scrolling.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_ActionZoomScroll : public RS_ActionInterface {
    Q_OBJECT
public:
    RS_ActionZoomScroll(RS2::Direction direction,
                        RS_EntityContainer& container,
                        RS_GraphicView& graphicView);
    ~RS_ActionZoomScroll() {}

    static RS_String getName() {
        return "Zoom Scroll";
    }

    virtual RS_String name() {
        return RS_ActionZoomScroll::getName();
    }


    virtual void init(int status=0);
    virtual void trigger();

protected:
    RS2::Direction direction;
};

#endif

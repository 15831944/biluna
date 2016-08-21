/****************************************************************************
** $Id: rs_actionmodifydelete.h 2487 2005-07-20 00:30:38Z andrew $
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

#ifndef RS_ACTIONMODIFYTOFRONT_H
#define RS_ACTIONMODIFYTOFRONT_H

#include "rs_actionmodifytoback.h"


/**
 * This action class can handle user events to move entities to
 * the background / foreground.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_ActionModifyToFront : public RS_ActionModifyToBack {
    Q_OBJECT
public:
    RS_ActionModifyToFront(RS_EntityContainer& container,
                          RS_GraphicView& graphicView);
    ~RS_ActionModifyToFront() {}

    static RS_String getName() {
        return "Modify To Front";
    }

    virtual RS_String name() {
        return RS_ActionModifyToFront::getName();
    }

    static RS_GuiAction* createGuiAction();
    static RS_ActionInterface* factory();
};

#endif

/****************************************************************************
** $Id: rs_actionselectinvert.h 10341 2008-04-06 17:41:55Z andrew $
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

#ifndef RS_ACTIONSELECTINVERT_H
#define RS_ACTIONSELECTINVERT_H

#include "rs_actionselectbase.h"


/**
 * This action class can handle user events to select all entities.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_ActionSelectInvert : public RS_ActionSelectBase {
    Q_OBJECT
public:
    RS_ActionSelectInvert(RS_EntityContainer& container,
                       RS_GraphicView& graphicView);
    ~RS_ActionSelectInvert() {}

    static RS_String getName() {
        return "Select Invert";
    }

    virtual RS_String name() {
        return RS_ActionSelectInvert::getName();
    }

    static RS_GuiAction* createGuiAction();
    static RS_ActionInterface* factory();

    void init(int status);
    virtual void trigger();
};

#endif

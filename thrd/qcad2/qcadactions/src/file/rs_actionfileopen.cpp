/****************************************************************************
** $Id: rs_actionfileopen.cpp 8821 2008-02-02 17:31:21Z andrew $
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

#include "rs_actionfileopen.h"

#include "rs_graphic.h"
//Added by qt3to4:
#include <QPixmap>



RS_ActionFileOpen::RS_ActionFileOpen(RS_EntityContainer& container,
                                     RS_GraphicView& graphicView)
        :RS_ActionInterface( container, graphicView) {}


RS_GuiAction* RS_ActionFileOpen::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionFileOpen::getName(), 
            "fileopen",
                                  tr("&Open..."));
        action->setShortcut("Ctrl+O");
        action->setStatusTip(tr("Opens an existing drawing"));
    }                                  
    return action;
}



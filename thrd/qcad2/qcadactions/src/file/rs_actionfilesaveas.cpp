/****************************************************************************
** $Id: rs_actionfilesaveas.cpp 8981 2008-02-11 08:34:10Z andrew $
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

#include "rs_actionfilesaveas.h"

#include "rs_graphic.h"
//Added by qt3to4:
#include <QPixmap>

RS_ActionFileSaveAs::RS_ActionFileSaveAs(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_ActionInterface( container, graphicView) {}


RS_GuiAction* RS_ActionFileSaveAs::createGuiAction() {
    static RS_GuiAction* action = NULL;

    if (action==NULL) {
        action = new RS_GuiAction(RS_ActionFileSaveAs::getName(), 
            "filesaveas", 
            tr("Save &as..."));
        action->setShortcut("Ctrl+Shift+S");
        action->setStatusTip(tr("Saves the current drawing under"
                                " a new filename"));
        
    }
    return action;
}

/*
void RS_ActionFileSaveAs::trigger() {
    RS_DEBUG->print("RS_ActionFileSaveAs::trigger");

    RS_String fileName; // = RS_DIALOGFACTORY->requestFileSaveAsDialog();
    if (graphic!=NULL && !fileName.isEmpty()) {
        graphic->saveAs(fileName, RS2::FormatUnknown);
    }
    finish();
}



void RS_ActionFileSaveAs::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}
*/

// EOF

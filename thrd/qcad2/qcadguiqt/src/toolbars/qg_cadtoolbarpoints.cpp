/****************************************************************************
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

#include "qg_cadtoolbarpoints.h"

#include <assert.h>

#include "qg_cadtoolbar.h"

#include "rs_actiondrawpoint.h"

/**
 * Constructor
 */
QG_CadToolBarPoints::QG_CadToolBarPoints(QG_CadToolBar* parent)
        : QG_CadToolBarBase(parent) {
    
    Ui::QG_CadToolBarPoints::setupUi(this);

    connect(bBack, SIGNAL(clicked()),
            this, SLOT(back()));

    //assert(actionHandler!=NULL);

    bPoint->setDefaultAction(
        RS_ActionDrawPoint::createGuiAction());
}


/**
 * Destructor
 */
QG_CadToolBarPoints::~QG_CadToolBarPoints() {}



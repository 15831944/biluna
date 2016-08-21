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

#ifndef QG_CADTOOLBARINFO_H
#define QG_CADTOOLBARINFO_H

#include "qg_cadtoolbarbase.h"

#include "ui_qg_cadtoolbarinfo.h"


/**
 *
 */
class CAD_EXPORT QG_CadToolBarInfo: public QG_CadToolBarBase, 
                          private Ui::QG_CadToolBarInfo {
    Q_OBJECT

public:
    QG_CadToolBarInfo(QG_CadToolBar* parent);
    virtual ~QG_CadToolBarInfo();
};

#endif


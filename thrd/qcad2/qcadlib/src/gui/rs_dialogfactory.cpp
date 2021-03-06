/****************************************************************************
** $Id: rs_dialogfactory.cpp 4585 2007-02-16 09:33:53Z andrew $
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


#include "rs_dialogfactory.h"
#include "rs_debug.h"

RS_DialogFactory* RS_DialogFactory::uniqueInstance = NULL;
    


/**
 * Private constructor.
 */
RS_DialogFactory::RS_DialogFactory() {
    RS_DEBUG->print("RS_DialogFacgory::RS_DialogFactory");
    factoryObject = NULL;
    RS_DEBUG->print("RS_DialogFacgory::RS_DialogFactory: OK");
}



/**
 * @return Instance to the unique font list.
 */
RS_DialogFactory* RS_DialogFactory::instance() {
    if (uniqueInstance==NULL) {
        uniqueInstance = new RS_DialogFactory();
    }

    return uniqueInstance;
}



/**
 * Sets the real factory object that can create and show dialogs.
 */
void RS_DialogFactory::setFactoryObject(RS_DialogFactoryInterface* fo) {
    RS_DEBUG->print("RS_DialogFactory::setFactoryObject");
    if (factoryObject!=NULL) {
        delete factoryObject;
    }
    factoryObject = fo;
    RS_DEBUG->print("RS_DialogFactory::setFactoryObject: OK");
}



/**
 * @return Factory object. This is never NULL. If no factory
 * object was set, the default adapter will be returned.
 */
RS_DialogFactoryInterface* RS_DialogFactory::getFactoryObject() {
    if (factoryObject!=NULL) {
        return factoryObject;
    } else {
        return &factoryAdapter;
    }
}

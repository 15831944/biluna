/****************************************************************************
** $Id: rs_undo.cpp 7067 2007-11-13 09:36:48Z andrew $
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


#include "rs_undo.h"



/**
 * Default constructor.
 */
RS_Undo::RS_Undo() {
    undoList.setAutoDelete(true);
    undoPointer = -1;
    currentCycle = NULL;
    undoEnabled = true;
}


RS_Undo::~RS_Undo() {
    if (currentCycle!=NULL) {
        delete currentCycle;
        currentCycle = NULL;
    }
}


/**
 * @return Number of Cycles that can be undone.
 */
int RS_Undo::countUndoCycles() {
    RS_DEBUG->print("RS_Undo::countUndoCycles");

    return undoPointer+1;
}



/**
 * @return Number of Cycles that can be redone.
 */
int RS_Undo::countRedoCycles() {
    RS_DEBUG->print("RS_Undo::countRedoCycles");

    return (int)undoList.count()-1-undoPointer;
}



/**
 * Adds an Undo Cycle at the current position in the list.
 * All Cycles after the new one are removed and the Undoabels
 * on them deleted.
 */
void RS_Undo::addUndoCycle(RS_UndoCycle* i) {
    RS_DEBUG->print("RS_Undo::addUndoCycle");
    
    if (!undoEnabled) {
        return;
    }
    
    undoList.insert(++undoPointer, i);
    
    RS_DEBUG->print("RS_Undo::addUndoCycle: ok");
}



/**
 * Starts a new cycle for one undo step. Every undoable that is
 * added after calling this method goes into this cycle.
 */
void RS_Undo::startUndoCycle() {
    RS_DEBUG->print("RS_Undo::startUndoCycle");
    
    if (!undoEnabled) {
        return;
    }

    // definitely delete Undo Cycles and all Undoables in them
    //   that cannot be redone now:
    while ((int)undoList.count()>undoPointer+1 && (int)undoList.count()>0) {
    
        RS_DEBUG->print("RS_Undo::startUndoCycle: "
            "deleting cycle in obsolete branch");

        RS_UndoCycle* l = undoList.last();

        if (l!=NULL) {
            RS_Undoable* u=NULL;
            bool done = false;
            do {
                u = l->getFirstUndoable();
                if (u!=NULL) {
                    // Remove the pointer from _all_ cycles:
                    removeUndoablePointers(u);

                    // Delete the Undoable for good:
                    RS_DEBUG->print("RS_Undo::startUndoCycle: delete undoable for good");
                    if (u->isUndone()) {
                        RS_DEBUG->print("RS_Undo::startUndoCycle: removing entity");
                        removeUndoable(u);
                        RS_DEBUG->print("RS_Undo::startUndoCycle: removing entity: OK");
                    }
                    RS_DEBUG->print("RS_Undo::startUndoCycle: delete undoable for good: OK");
                } else {
                    done = true;
                }
            } while(!done);
        }

        // remove obsolete cycle for good
        RS_DEBUG->print("RS_Undo::startUndoCycle: removing undo cycle");
        undoList.removeLast();
        RS_DEBUG->print("RS_Undo::startUndoCycle: removing undo cycle: OK");
    }

    currentCycle = new RS_UndoCycle();
    
    RS_DEBUG->print("RS_Undo::startUndoCycle: OK");
}
    
    
    
void RS_Undo::removeUndoablePointers(RS_Undoable* u) {
    for (int i=0; i<undoList.size(); ++i) {
        RS_DEBUG->print("RS_Undo::startUndoCycle: removing entity from list");
        undoList[i]->removeUndoable(u);
        RS_DEBUG->print("RS_Undo::startUndoCycle: removing entity from list: OK");
    }
}



/**
 * Adds an undoable to the current undo cycle.
 */
void RS_Undo::addUndoable(RS_Undoable* u) {
    RS_DEBUG->print("RS_Undo::addUndoable");
    
    if (!undoEnabled) {
        return;
    }
    
    if (currentCycle!=NULL) {
        currentCycle->addUndoable(u);
    } else {
        RS_DEBUG->print(RS_Debug::D_WARNING,
            "RS_Undo::addUndoable(): No undo cycle active.");
    }
}



/**
 * Ends the current undo cycle.
 */
void RS_Undo::endUndoCycle() {
    
    if (!undoEnabled) {
        return;
    }

    addUndoCycle(currentCycle);
    currentCycle = NULL;
}



/**
 * Undoes the last undo cycle.
 */
void RS_Undo::undo() {
    RS_DEBUG->print("RS_Undo::undo");
    
    if (!undoEnabled) {
        return;
    }

    if (undoPointer>=0) {
        RS_UndoCycle* i = undoList.at(undoPointer);
        if (i!=NULL) {
            for (int k=0; k<i->undoables.size(); ++k) {
                i->undoables[k]->changeUndoState();
            }
            undoPointer--;
        }
    }
}



/**
 * Redoes the undo cycle which was at last undone.
 */
void RS_Undo::redo() {
    RS_DEBUG->print("RS_Undo::redo");
    
    if (!undoEnabled) {
        return;
    }

    if (undoPointer+1<(int)undoList.count()) {
        undoPointer++;
        RS_UndoCycle* i = undoList.at(undoPointer);
        if (i!=NULL) {
            for (int k=0; k<i->undoables.size(); ++k) {
                i->undoables[k]->changeUndoState();
            }
        }
    }
}



/**
 * @return The undo item that is next if we're about to undo
 * or NULL.
 */
RS_UndoCycle* RS_Undo::getUndoCycle() {
    RS_UndoCycle* ret = NULL;
    
    if (!undoEnabled) {
        return ret;
    }
    
    RS_DEBUG->print("RS_Undo::getUndoCycle");

    if (undoPointer>=0 && undoPointer<(int)undoList.count()) {
        ret = undoList.at(undoPointer);
    }
    RS_DEBUG->print("RS_Undo::getUndoCycle: OK");

    return ret;
}



/**
 * @return The redo item that is next if we're about to redo
 * or NULL.
 */
RS_UndoCycle* RS_Undo::getRedoCycle() {
    RS_UndoCycle* ret = NULL;

    RS_DEBUG->print("RS_Undo::getRedoCycle");
    
    if (!undoEnabled) {
        return ret;
    }

    if (undoPointer+1>=0 && undoPointer+1<(int)undoList.count()) {
        ret =  undoList.at(undoPointer+1);
    }

    return ret;
}


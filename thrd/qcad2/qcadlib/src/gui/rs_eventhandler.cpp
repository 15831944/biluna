/****************************************************************************
** $Id: rs_eventhandler.cpp 10610 2008-05-02 12:14:52Z andrew $
**
** Copyright (C) 2001-2008 RibbonSoft. All rights reserved.
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


#include "rs_eventhandler.h"

#include "rs_actioninterface.h"
#include "rs_coordinateevent.h"
#include "rs_regexp.h"
//Added by qt3to4:
#include <QEvent>

/**
 * Constructor.
 */
RS_EventHandler::RS_EventHandler(RS_GraphicView* graphicView) {
    this->graphicView = graphicView;
    actionIndex=-1;
    for (int i=0; i<RS_MAXACTIONS; ++i) {
        currentActions[i] = NULL;
    }
    coordinateInputEnabled = true;
    defaultAction = NULL;
}



/**
 * Destructor.
 */
RS_EventHandler::~RS_EventHandler() {
    RS_DEBUG->print("RS_EventHandler::~RS_EventHandler");
    if (defaultAction!=NULL) {
        defaultAction->finish();
        delete defaultAction;
        defaultAction = NULL;
    }

    killAllActions();
   
    /*
    RS_DEBUG->print("RS_EventHandler::~RS_EventHandler: Deleting all actions..");
    for (int i=0; i<RS_MAXACTIONS; ++i) {
        if (currentActions[i]!=NULL) {
            currentActions[i]->setFinished();
            //delete currentActions[i];
            //currentActions[i] = NULL;
        }
    }
    cleanUp();
    RS_DEBUG->print("RS_EventHandler::~RS_EventHandler: Deleting all actions..: OK");
    */

    RS_DEBUG->print("RS_EventHandler::~RS_EventHandler: OK");
}


/**
 * Go back in current action.
 */
void RS_EventHandler::back() {
    RS_MouseEvent e(QEvent::MouseButtonRelease, QPoint(0,0),
                    Qt::RightButton, Qt::MouseButtons(Qt::RightButton),
                    Qt::KeyboardModifiers(Qt::NoModifier));
    mouseReleaseEvent(&e);
}



/**
 * Go enter pressed event for current action.
 */
void RS_EventHandler::enter() {
    RS_KeyEvent e(QEvent::KeyPress, Qt::Key_Enter,
                  Qt::KeyboardModifiers(Qt::NoModifier));
    keyPressEvent(&e);
}


/**
 * Called by RS_GraphicView 
 */
void RS_EventHandler::mousePressEvent(RS_MouseEvent* e) {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->mousePressEvent(e);
        e->accept();
    } else {
        if (defaultAction!=NULL) {
            defaultAction->mousePressEvent(e);
            e->accept();
        } else {
            RS_DEBUG->print("currently no action defined");
            e->ignore();
        }
    }
}



/**
 * Called by RS_GraphicView 
 */
void RS_EventHandler::mouseReleaseEvent(RS_MouseEvent* e) {

    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {

        currentActions[actionIndex]->mouseReleaseEvent(e);

        // Clean up actions - one might be finished now
        cleanUp();
        e->accept();
    } else {
        if (defaultAction!=NULL) {
            defaultAction->mouseReleaseEvent(e);
        } else {
            e->ignore();
        }
    }
}



/**
 * Called by RS_GraphicView 
 */
void RS_EventHandler::mouseMoveEvent(RS_MouseEvent* e) {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->mouseMoveEvent(e);
        e->accept();
    } else {
        if (defaultAction!=NULL) {
            defaultAction->mouseMoveEvent(e);
            e->accept();
        } else {
            e->ignore();
        }
        //RS_DEBUG->print("currently no action defined");
    }
}



/**
 * Called by RS_GraphicView 
 */
void RS_EventHandler::mouseLeaveEvent() {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->suspend();
    } else {
        if (defaultAction!=NULL) {
            defaultAction->suspend();
        }
        //RS_DEBUG->print("currently no action defined");
    }
}



/**
 * Called by RS_GraphicView 
 */
void RS_EventHandler::mouseEnterEvent() {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->resume();
    } else {
        if (defaultAction!=NULL) {
            defaultAction->resume();
        }
    }
}



/**
 * Called by RS_GraphicView 
 */
void RS_EventHandler::keyPressEvent(RS_KeyEvent* e) {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->keyPressEvent(e);
    } else {
        if (defaultAction!=NULL) {
            defaultAction->keyPressEvent(e);
        } else {
#if QT_VERSION>=0x030000
            e->ignore();
#endif

        }

        //RS_DEBUG->print("currently no action defined");
    }
}



/**
 * Called by RS_GraphicView 
 */
void RS_EventHandler::keyReleaseEvent(RS_KeyEvent* e) {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->keyReleaseEvent(e);
    } else {
        if (defaultAction!=NULL) {
            defaultAction->keyReleaseEvent(e);
        } else {
            e->ignore();
        }
        //RS_DEBUG->print("currently no action defined");
    }
}
    
    
    
void RS_EventHandler::dragEnterEvent(QDragEnterEvent* e) {
    RS_DEBUG->print("RS_EventHandler::dragEnterEvent");
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->dragEnterEvent(e);
    } else {
        if (defaultAction!=NULL) {
            defaultAction->dragEnterEvent(e);
        } else {
            e->ignore();
        }
    }
}


void RS_EventHandler::dragMoveEvent(QDragMoveEvent* e) {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->dragMoveEvent(e);
    } else {
        if (defaultAction!=NULL) {
            defaultAction->dragMoveEvent(e);
        } else {
            e->ignore();
        }
    }
}




void RS_EventHandler::dropEvent(QDropEvent* e) {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {
        currentActions[actionIndex]->dropEvent(e);
    } else {
        if (defaultAction!=NULL) {
            defaultAction->dropEvent(e);
        } else {
            e->ignore();
        }
    }
}



/**
 * Handles command line events.
 */
void RS_EventHandler::commandEvent(RS_CommandEvent* e) {
    RS_DEBUG->print("RS_EventHandler::commandEvent");

    //RS_RegExp rex;
    RS_String cmd = e->getCommand();

    if (coordinateInputEnabled) {
        if (!e->isAccepted()) {
            // handle absolute cartesian coordinate input:
            if (cmd.contains(',') && cmd.at(0)!='@') {
                if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
                        !currentActions[actionIndex]->isFinished()) {
                    int commaPos = cmd.indexOf(',');
                    bool ok1, ok2;
                    double x = RS_Math::eval(cmd.left(commaPos), &ok1);
                    double y = RS_Math::eval(cmd.mid(commaPos+1), &ok2);

                    if (ok1 && ok2) {
                        RS_CoordinateEvent ce(RS_Vector(x,y));
                        currentActions[actionIndex]->coordinateEvent(&ce);
                    } else {
                        if (RS_DIALOGFACTORY!=NULL) {
                            RS_DIALOGFACTORY->commandMessage(
                                "Expression Syntax Error");
                        }
                    }
                    e->accept();
                }
            }
        }

        // handle relative cartesian coordinate input:
        if (!e->isAccepted()) {
            if (cmd.contains(',') && cmd.at(0)=='@') {
                if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
                        !currentActions[actionIndex]->isFinished()) {
                    int commaPos = cmd.indexOf(',');
                    bool ok1, ok2;
                    double x = RS_Math::eval(cmd.mid(1, commaPos-1), &ok1);
                    double y = RS_Math::eval(cmd.mid(commaPos+1), &ok2);

                    if (ok1 && ok2) {
                        RS_CoordinateEvent ce(RS_Vector(x,y) +
                                              graphicView->getRelativeZero());
                        currentActions[actionIndex]->coordinateEvent(&ce);
                    } else {
                        if (RS_DIALOGFACTORY!=NULL) {
                            RS_DIALOGFACTORY->commandMessage(
                                "Expression Syntax Error");
                        }
                    }
                    e->accept();
                }
            }
        }

        // handle absolute polar coordinate input:
        if (!e->isAccepted()) {
            if (cmd.contains('<') && cmd.at(0)!='@') {
                if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
                        !currentActions[actionIndex]->isFinished()) {
                    int commaPos = cmd.indexOf('<');
                    bool ok1, ok2;
                    double r = RS_Math::eval(cmd.left(commaPos), &ok1);
                    double a = RS_Math::eval(cmd.mid(commaPos+1), &ok2);

                    if (ok1 && ok2) {
                        RS_Vector pos;
                        pos.setPolar(r,RS_Math::deg2rad(a));
                        RS_CoordinateEvent ce(pos);
                        currentActions[actionIndex]->coordinateEvent(&ce);
                    } else {
                        if (RS_DIALOGFACTORY!=NULL) {
                            RS_DIALOGFACTORY->commandMessage(
                                "Expression Syntax Error");
                        }
                    }
                    e->accept();
                }
            }
        }

        // handle relative polar coordinate input:
        if (!e->isAccepted()) {
            if (cmd.contains('<') && cmd.at(0)=='@') {
                if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
                        !currentActions[actionIndex]->isFinished()) {
                    int commaPos = cmd.indexOf('<');
                    bool ok1, ok2;
                    double r = RS_Math::eval(cmd.mid(1, commaPos-1), &ok1);
                    double a = RS_Math::eval(cmd.mid(commaPos+1), &ok2);

                    if (ok1 && ok2) {
                        RS_Vector pos;
                        pos.setPolar(r,RS_Math::deg2rad(a));
                        RS_CoordinateEvent ce(pos +
                                              graphicView->getRelativeZero());
                        currentActions[actionIndex]->coordinateEvent(&ce);
                    } else {
                        if (RS_DIALOGFACTORY!=NULL) {
                            RS_DIALOGFACTORY->commandMessage(
                                "Expression Syntax Error");
                        }
                    }
                    e->accept();
                }
            }
        }
    }

    // send command event directly to current action:
    if (!e->isAccepted()) {
        if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
                !currentActions[actionIndex]->isFinished()) {
            currentActions[actionIndex]->commandEvent(e);
            //e->accept();
        } else {
            if (defaultAction!=NULL) {
                defaultAction->commandEvent(e);
                //e->accept();
            }
        }
    }

    RS_DEBUG->print("RS_EventHandler::commandEvent: OK");
}



/**
 * Handles coordinate events (e.g. from coordinate input widgets).
 */
void RS_EventHandler::coordinateEvent(RS_CoordinateEvent* e) {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {

        currentActions[actionIndex]->coordinateEvent(e);
    }
}


/**
 * Enables coordinate input in the command line.
 */
void RS_EventHandler::enableCoordinateInput() {
    coordinateInputEnabled = true;
}



/**
 * Enables coordinate input in the command line.
 */
void RS_EventHandler::disableCoordinateInput() {
    coordinateInputEnabled = false;
}



/**
 * @return Current action.
 */
RS_ActionInterface* RS_EventHandler::getCurrentAction() {
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
            !currentActions[actionIndex]->isFinished()) {

        return currentActions[actionIndex];
    } else {
        return defaultAction;
    }
}



/**
 * @return The current default action.
 */
RS_ActionInterface* RS_EventHandler::getDefaultAction() {
    return defaultAction;
}



/**
 * Sets the default action.
 */
void RS_EventHandler::setDefaultAction(RS_ActionInterface* action) {
    if (defaultAction!=NULL) {
        defaultAction->finish();
        delete defaultAction;
        defaultAction = NULL;
    }

    defaultAction = action;
}



/**
 * Sets the current action.
 */
void RS_EventHandler::setCurrentAction(RS_ActionInterface* action) {
    RS_DEBUG->print("RS_EventHandler::setCurrentAction");
    if (action==NULL) {
        return;
    }

    if (action->isExclusive()) {
        killAllActions();
    }

    // Predecessor of the new action or NULL:
    RS_ActionInterface* predecessor = NULL;

    // Suspend current action:
    if (actionIndex>=0 && currentActions[actionIndex]!=NULL &&
        !currentActions[actionIndex]->isFinished()) {

        predecessor = currentActions[actionIndex];
        predecessor->suspend();
        predecessor->hideOptions();
    } else {
        if (defaultAction!=NULL) {
            predecessor = defaultAction;
            predecessor->suspend();
            predecessor->hideOptions();
        }
    }

    // Forget about the oldest action and make space for the new action:
    if (actionIndex==RS_MAXACTIONS-1) {
        // delete oldest action if necessary (usually never happens):
        if (currentActions[0]!=NULL) {
            currentActions[0]->finish();
            delete currentActions[0];
            currentActions[0] = NULL;
        }
        // Move up actionstack (optimize):
        for (int i=0; i<RS_MAXACTIONS-1; ++i) {
            currentActions[i] = currentActions[i+1];
        }
    } else if (actionIndex<RS_MAXACTIONS-1) {
        actionIndex++;
    }

    // Set current action:
    currentActions[actionIndex] = action;

    // Initialisation of our new action:
    RS_DEBUG->print("RS_EventHandler::setCurrentAction: init current action");
    action->init();
    action->init(0);

    if (action->isFinished()==false) {
        RS_DEBUG->print("RS_EventHandler::setCurrentAction: show options");
        currentActions[actionIndex]->showOptions();
        RS_DEBUG->print("RS_EventHandler::setCurrentAction: set predecessor");
        action->setPredecessor(predecessor);
    }

    RS_DEBUG->print("RS_EventHandler::setCurrentAction: cleaning up..");
    cleanUp();

    RS_DEBUG->print("RS_EventHandler::setCurrentAction: debugging actions");
    //debugActions();
    RS_DEBUG->print("RS_EventHandler::setCurrentAction: OK");
}



/**
 * Kills all running selection actions. Called when a selection action
 * is launched to reduce confusion.
 */
/*
void RS_EventHandler::killSelectActions() {
    RS_DEBUG->print("RS_EventHandler::killSelectActions");
    for (int c=0; c<RS_MAXACTIONS; ++c) {
        if (currentActions[c]!=NULL) {
            if (dynamic_cast<RS_ActionSelectBase*>currentActions[c]!=NULL) {
                currentActions[c]->finish();
            }
        }
    }
}
*/



/**
 * Kills all running actions.
 */
void RS_EventHandler::killAllActions() {
   for (int c=0; c<RS_MAXACTIONS; ++c) {
       if (currentActions[c]!=NULL && !currentActions[c]->isPersistent()) {
           currentActions[c]->finish();
           //currentActions[c]->setFinished();
       }
   }
   actionIndex = -1;
   //cleanUp();
}



/**
 * @return true if there is at least one action in the action stack.
 */
bool RS_EventHandler::hasAction() {
    if (actionIndex!=-1 || defaultAction!=NULL) {
        return true;
    } else {
        return false;
    }
}



/**
 * Garbage collector for actions.
 */
void RS_EventHandler::cleanUp() {
    RS_DEBUG->print("RS_EventHandler::cleanUp");

    int o=0;   // old index
    int n=0;   // new index
    int resume=0; // index of action to resume
    bool doResume=false; // do we need to resume an action
    actionIndex = -1;

    //debugActions();
    do {
        // search first used action (o)
        while (currentActions[o]==NULL && o<RS_MAXACTIONS) {
            o++;
        }

        // delete action if it is finished
        if (o<RS_MAXACTIONS && currentActions[o]!=NULL &&
            currentActions[o]->isFinished()) {
            delete currentActions[o];
            currentActions[o] = NULL;

            doResume = true;
        }

        // move a running action up in the stack
        if (o<RS_MAXACTIONS && currentActions[o]!=NULL) {
            if (n!=o) {
                currentActions[n] = currentActions[o];
                resume = n;
                currentActions[o] = NULL;
            } else {
                if (o<RS_MAXACTIONS) {
                    o++;
                }
            }
            actionIndex = n;
            if (n<RS_MAXACTIONS-1) {
                n++;
            }
        }
    } while (o<RS_MAXACTIONS);
        
    //debugActions();

    // Resume last used action:
    if (doResume) {
        if (currentActions[resume]!=NULL &&
            !currentActions[resume]->isFinished()) {
        
            currentActions[resume]->resume();
            currentActions[resume]->showOptions();
        } else {
            if (defaultAction!=NULL) {
                defaultAction->resume();
                defaultAction->showOptions();
            }
        }
    }
    RS_DEBUG->print("RS_EventHandler::cleanUp: OK");
}



/**
 * Sets the snap mode for all currently active actions.
 */
void RS_EventHandler::setSnapMode(RS2::ActionType sm) {
    for (int c=0; c<RS_MAXACTIONS; ++c) {
        if (currentActions[c]!=NULL) {
            currentActions[c]->setSnapMode(sm);
        }
    }

    if (defaultAction!=NULL) {
        defaultAction->setSnapMode(sm);
    }
}



/**
 * Sets the snap restriction for all currently active actions.
 */
void RS_EventHandler::setSnapRestriction(RS2::ActionType sr) {
    for (int c=0; c<RS_MAXACTIONS; ++c) {
        if (currentActions[c]!=NULL) {
            currentActions[c]->setSnapRestriction(sr);
        }
    }

    if (defaultAction!=NULL) {
        defaultAction->setSnapRestriction(sr);
    }
}


void RS_EventHandler::debugActions() {
    RS_DEBUG->print("---");
    for (int c=0; c<RS_MAXACTIONS; ++c) {
        if (c==actionIndex) {
            RS_DEBUG->print("Current");
        }
        if (currentActions[c]!=NULL) {
            RS_DEBUG->print("Action %03d: %s [%s]",
                            c, 
                            (const char*)currentActions[c]->name().toLatin1(),
                            currentActions[c]->isFinished() ? "finished" : "active");
        } else {
            RS_DEBUG->print("Action %03d: NULL", c);
        }
    }
}


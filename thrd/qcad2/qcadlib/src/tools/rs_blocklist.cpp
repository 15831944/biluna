/****************************************************************************
** $Id: rs_blocklist.cpp 4861 2007-03-07 15:29:43Z andrew $
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


#include "rs_blocklist.h"


/**
 * Constructor.
 * 
 * @param owner true if this is the owner of the blocks added.
 *              If so, the blocks will be deleted when the block
 *              list is deleted.
 */
RS_BlockList::RS_BlockList(bool owner) {
    RS_DEBUG->print("RS_BlockList::RS_BlockList: owner: %d", (int)owner);
    this->owner = owner;
    // no auto delete, so we can notify listeners on remove first
    blocks.setAutoDelete(false);
    blockListListeners.setAutoDelete(false);
    activeBlock = NULL;
    setModified(false);
}


/**
 * Removes all blocks in the blocklist.
 */
void RS_BlockList::clear() {
    RS_DEBUG->print("RS_BlockList::clear");
    if (owner) {
        blocks.setAutoDelete(true);
    }

    blocks.clear();
    
    if (owner) {
        blocks.setAutoDelete(false);
    }
    setModified(true);
}


/**
 * Activates the given block.
 * Listeners are notified.
 */
void RS_BlockList::activate(const RS_String& name) {
    RS_DEBUG->print("RS_BlockList::activate: %s", 
        (const char*)name.toLatin1());

    activate(find(name));
}

/**
 * Activates the given block.
 * Listeners are notified.
 */
void RS_BlockList::activate(RS_Block* block) {
    RS_DEBUG->print("RS_BlockList::activate");
    activeBlock = block;

    /*
       for (uint i=0; i<blockListListeners.count(); ++i) {
           RS_BlockListListener* l = blockListListeners.at(i);
        if (l!=NULL) {
               l->blockActivated(activeBlock);
        }
       }
    */

    RS_DEBUG->print("RS_BlockList::activate: OK");
}


/**
 * Adds a block to the block list. If a block with the same name
 * exists already, the given block will be deleted if the blocklist
 * owns the blocks.
 *
 * @param notify true if you want listeners to be notified.
 *
 * @return false: block already existed and was deleted.
 */
bool RS_BlockList::add(RS_Block* block, bool notify) {
    RS_DEBUG->print("RS_BlockList::add()");

    if (block==NULL) {
        return false;
    }

    // check if block already exists:
    RS_Block* b = find(block->getName());
    if (b==NULL) {
        blocks.append(block);

        if (notify) {
            addNotification(block);
        }
        setModified(true);

        return true;
    } else {
        if (owner) {
            delete block;
            block = NULL;
        }
        return false;
    }

}



/**
 * Notifies the listeners about blocks that were added. This can be
 * used after adding a lot of blocks without auto-update or simply
 * to force an update of GUI blocklists.
 */
void RS_BlockList::addNotification(RS_Block* block) {
    for (int i=0; i<blockListListeners.count(); ++i) {
        RS_BlockListListener* l = blockListListeners.at(i);
        l->blockAdded(block);
    }
}



/**
 * Removes a block from the list.
 * Listeners are notified after the block was removed from 
 * the list but before it gets deleted.
 */
void RS_BlockList::remove(RS_Block* block) {
    RS_DEBUG->print("RS_BlockList::removeBlock");
    
    // activate an other block if necessary:
    if (activeBlock==block) {
        //activate(blocks.first());
        activeBlock = NULL;
    }

    // here the block is removed from the list but not deleted
    blocks.removeAt(blocks.indexOf(block));

    RS_DEBUG->print("RS_BlockList::removeBlock: notifying listeners");
    for (int i=0; i<blockListListeners.count(); ++i) {
        RS_BlockListListener* l = blockListListeners.at(i);
        l->blockRemoved(block);
    }
    RS_DEBUG->print("RS_BlockList::removeBlock: notifying listeners: OK");
        
    setModified(true);

    // now it's save to delete the block
    if (owner) {
        RS_DEBUG->print("RS_BlockList::removeBlock: deleting block");
        delete block;
        RS_DEBUG->print("RS_BlockList::removeBlock: deleting block: OK");
    }
    
    RS_DEBUG->print("RS_BlockList::removeBlock: OK");
}



/**
 * Tries to rename the given block to 'name'. Block names are unique in the
 * block list.
 *
 * @retval true block was successfully renamed.
 * @retval false block couldn't be renamed.
 */
bool RS_BlockList::rename(RS_Block* block, const RS_String& name) {
    if (block!=NULL) {
        if (find(name)==NULL) {
            block->setName(name);
            setModified(true);
            return true;
        }
    }

    return false;
}


/**
 * Changes a block's attributes. The attributes of block 'block'
 * are copied from block 'source'.
 * Listeners are notified.
 */
/*
void RS_BlockList::editBlock(RS_Block* block, const RS_Block& source) {
    *block = source;
    
    for (uint i=0; i<blockListListeners.count(); ++i) {
        RS_BlockListListener* l = blockListListeners.at(i);
 
        l->blockEdited(block);
    }
}
*/



/**
 * @return Pointer to the block with the given name or
 * \p NULL if no such block was found.
 */
RS_Block* RS_BlockList::find(const RS_String& name) {
    //RS_DEBUG->print("RS_BlockList::find");
    RS_Block* ret = NULL;

    for (int i=0; i<count(); ++i) {
        RS_Block* b = at(i);
        if (b->getName()==name) {
            ret = b;
            break;
        }
    }

    return ret;
}



/**
 * Finds a new unique block name.
 *
 * @param suggestion Suggested name the new name will be based on.
 */
RS_String RS_BlockList::newName(const RS_String& suggestion) {
    RS_String name;
    for (int i=0; i<1e5; ++i) {
        name = RS_String("%1-%2").arg(suggestion).arg(i);
        if (find(name)==NULL) {
            return name;
        }
    }

    return "0";
}



/**
 * Switches on / off the given block. 
 * Listeners are notified.
 */
void RS_BlockList::toggle(const RS_String& name) {
    toggle(find(name));
}



/**
 * Switches on / off the given block. 
 * Listeners are notified.
 */
void RS_BlockList::toggle(RS_Block* block) {
    if (block==NULL) {
        return;
    }

    block->toggle();

    // Notify listeners:
    for (int i=0; i<blockListListeners.count(); ++i) {
        RS_BlockListListener* l = blockListListeners.at(i);

        l->blockToggled(block);
    }
}



/**
 * Freezes or defreezes all blocks.
 *
 * @param freeze true: freeze, false: defreeze
 */
void RS_BlockList::freezeAll(bool freeze) {

    for (int l=0; l<count(); l++) {
        at(l)->freeze(freeze);
    }

    for (int i=0; i<blockListListeners.count(); ++i) {
        RS_BlockListListener* l = blockListListeners.at(i);
        l->blockToggled(NULL);
    }
}



/**
 * Switches on / off the given block. 
 * Listeners are notified.
 */
/*
void RS_BlockList::toggleBlock(const RS_String& name) {
    RS_Block* block = findBlock(name);
    block->toggle();
    
    // Notify listeners:
    for (uint i=0; i<blockListListeners.count(); ++i) {
        RS_BlockListListener* l = blockListListeners.at(i);
 
        l->blockToggled(block);
    }
}
*/


/**
 * adds a BlockListListener to the list of listeners. Listeners
 * are notified when the block list changes.
 */
void RS_BlockList::addListener(RS_BlockListListener* listener) {
    if (listener!=NULL) {
        blockListListeners.append(listener);
    }
}



/**
 * removes a BlockListListener from the list of listeners. 
 */
void RS_BlockList::removeListener(RS_BlockListListener* listener) {
    if (listener!=NULL) {
        blockListListeners.removeAt(blockListListeners.indexOf(listener));
    }
}


#include "RTransactionStack.h"

#include "RDebug.h"
#include "RDocument.h"
#include "RTransaction.h"
#include "RStorage.h"



RTransactionStack::RTransactionStack(RDocument& document) 
    : document(document) {
}

RTransactionStack::~RTransactionStack() {
}

void RTransactionStack::reset() {
	RStorage& storage = document.getStorage();
	storage.resetTransactionStack();
}

bool RTransactionStack::isUndoAvailable() const {
	RStorage& storage = document.getStorage();
	int lastTransactionId = storage.getLastTransactionId();

	if (lastTransactionId < 0) {
		return false;
	}
	return true;
}

bool RTransactionStack::isRedoAvailable() const {
	RStorage& storage = document.getStorage();
	int lastTransactionId = storage.getLastTransactionId();
	int maxTransactionId = storage.getMaxTransactionId();
	if (lastTransactionId >= maxTransactionId) {
		return false;
	}
	return true;
}



/**
 * Undoes the last transaction.
 *
 * \return Transaction that was undone. The transaction contains a set of 
 *   affected entity IDs. Higher level callers can use
 *   this set to update scenes, views, etc accordingly.
 */
RTransaction RTransactionStack::undo() {
    RStorage& storage = document.getStorage();
    int lastTransactionId = storage.getLastTransactionId();

    if (lastTransactionId<0) {
        RDebug::warning("RTransactionStack::undo: "
            "already at top of transaction stack");
        return RTransaction(storage);
    }

    RTransaction lastTransaction = storage.getTransaction(lastTransactionId);

    // move up in transaction log:
    storage.setLastTransactionId(lastTransactionId-1);

    lastTransaction.undo(&document);

    return lastTransaction;
}



/**
 * Redoes the last transaction.
 *
 * \return Set of affected entity IDs. Higher level callers can use
 *   this set to update scenes, views, etc accordingly.
 */
RTransaction RTransactionStack::redo() {
    RStorage& storage = document.getStorage();
    int lastTransactionId = storage.getLastTransactionId();

    if (lastTransactionId >= storage.getMaxTransactionId()) {
		RDebug::warning("RTransactionStack::redo: "
			"already at bottom of transaction stack");
		return RTransaction(storage);
	}

    // move down in transaction log:
    storage.setLastTransactionId(lastTransactionId+1);
    
    RTransaction lastTransaction = storage.getTransaction(lastTransactionId+1);

    // update entity undo status:
    //std::set<REntity::Id> affectedEntities = lastTransaction.getAffectedEntities();
    //storage.toggleUndoStatus(affectedEntities);
    lastTransaction.redo(&document);

    return lastTransaction;
}

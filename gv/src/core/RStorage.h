#ifndef RSTORAGE_H
#define RSTORAGE_H

#include <QString>
#include <QSharedPointer>

#include "RBlock.h"
#include "REntity.h"
#include "RLayer.h"
#include "RLinetype.h"
#include "RNonCopyable.h"
#include "RRequireHeap.h"
#include "RTransaction.h"
#include "RUcs.h"
#include "RView.h"


/**
 * This is the abstract base class for all storage implementations.
 * A storage is used to store documents (entities, layers, blocks,
 * transactions) in memory or on disk. Every \ref RDocument "document"
 * is backed by a storage object.
 *
 * A storage implementation is available in the \ref qcaddbstorage.
 *
 * \ingroup core
 * \scriptable
 */
class RStorage : RNonCopyable, public RRequireHeap{
public:
    RStorage();
    ~RStorage() { }

    /**
     * Resets this storage to its initial, empty state.
     */
    virtual void clear() = 0;

    /**
     * Starts a new transaction.
     * This function is called before something is stored in the
     * storage.
     */
    virtual void beginTransaction() = 0;

    /**
     * Commits the current transaction.
     * This function is called after something has been stored in the
     * storage.
     */
    virtual void commitTransaction() = 0;

    /**
     * Rolls the current transaction back in order to cancel it.
     * This function should never be called under normal circumstances.
     */
    virtual void rollbackTransaction() = 0;

    /**
     * \return Ordered list of given IDs for display purposes.
     */
    virtual QList<REntity::Id> orderBackToFront(const QSet<REntity::Id>& entityIds) = 0;

    /**
     * \return A set of all object IDs of the document.
     */
    virtual QSet<RObject::Id> queryAllObjects() = 0;

    /**
     * \return A set of all entity IDs of the document.
     */
    virtual QSet<REntity::Id> queryAllEntities(bool undone = false,
			bool allBlocks = false) = 0;

    /**
     * \return A set of all UCS IDs of the document.
     */
    virtual QSet<RUcs::Id> queryAllUcs() = 0;

    /**
     * \return A set of all layer IDs of the document.
     */
    virtual QSet<RLayer::Id> queryAllLayers(bool undone = false) = 0;

    /**
     * \return A set of all block IDs of the document.
     */
    virtual QSet<RBlock::Id> queryAllBlocks(bool undone = false) = 0;

    /**
     * \return A set of all view IDs of the document.
     */
    virtual QSet<RView::Id> queryAllViews(bool undone = false) = 0;

    /**
     * \return A set of all linetype IDs of the document.
     */
    virtual QSet<RLinetype::Id> queryAllLinetypes() = 0;

    /**
     * \return A set of all entity IDs on the given layer.
     */
    virtual QSet<REntity::Id> queryLayerEntities(RLayer::Id layerId) = 0;

    /**
     * \return A set of all entity IDs that are part of the given block.
     */
    virtual QSet<REntity::Id> queryBlockEntities(RBlock::Id blockId) = 0;

    /**
     * \return A set of all block reference entity IDs that reference
     * the given block.
     */
    virtual QSet<REntity::Id> queryBlockReferences(RBlock::Id blockId) = 0;

    /**
     * \return A set of entity IDs of all selected entities.
     */
    virtual QSet<REntity::Id> querySelectedEntities() = 0;

    /**
     * \return A shared pointer to the object with the given \c objectId
     *      or null pointer if the object is no available in this storage.
     */
    virtual QSharedPointer<RObject> queryObject(RObject::Id objectId) = 0;

    virtual QSharedPointer<RObject> queryObjectDirect(RObject::Id objectId) {
		return queryObject(objectId);
	}

    /**
     * \return A pointer to the enitity with the given \c entityId
     *      or NULL if no such entity exists.
     */
    virtual QSharedPointer<REntity> queryEntity(REntity::Id entityId) = 0;

    virtual QSharedPointer<REntity> queryEntityDirect(REntity::Id entityId) {
		return queryEntity(entityId);
	}

    /**
     * \return A pointer to the UCS with the given \c ucsId
     *      or NULL if no such UCS exists.
     */
    virtual QSharedPointer<RUcs> queryUcs(RUcs::Id ucsId) = 0;

    /**
     * \return A pointer to the UCS with the given \c ucsName
     *      or NULL if no such UCS exists.
     */
    virtual QSharedPointer<RUcs> queryUcs(const QString& ucsName) = 0;

    /**
     * \return A pointer to the layer with the given \c layerId
     *      or NULL if no such layer exists.
     */
    virtual QSharedPointer<RLayer> queryLayer(RLayer::Id layerId) = 0;

    /**
     * \return A pointer to the layer with the given \c layerName
     *      or NULL if no such layer exists.
     */
    virtual QSharedPointer<RLayer> queryLayer(const QString& layerName) = 0;

    virtual QSharedPointer<RLayer> queryCurrentLayer() {
        return queryLayer(getCurrentLayerId());
    }

    virtual QSharedPointer<RView> queryCurrentView() {
        return queryView(getCurrentViewId());
    }

    virtual QSharedPointer<RLinetype>
			queryLinetype(RLinetype::Id linetypeId) = 0;

	virtual QSharedPointer<RLinetype>
			queryLinetype(const QString& linetypeName) = 0;

    virtual QSharedPointer<RBlock> queryBlock(RBlock::Id blockId) = 0;
    virtual QSharedPointer<RBlock> queryBlock(const QString& blockName) = 0;

    virtual QSharedPointer<RView> queryView(RView::Id viewId) = 0;
    virtual QSharedPointer<RView> queryView(const QString& viewName) = 0;

    void setCurrentLayer(RLayer::Id layerId) {
        setVariable("CurrentLayer", layerId);
    }

    void setCurrentLayer(const QString& layerName) {
        RLayer::Id id = getLayerId(layerName);
        if (id == RLayer::INVALID_ID) {
            return;
        }
        setCurrentLayer(id);
    }

    RLayer::Id getCurrentLayerId() {
        return (RLayer::Id) getVariable("CurrentLayer").toInt();
    }

    RView::Id getCurrentViewId() {
        return (RView::Id) getVariable("CurrentView").toInt();
    }

    virtual void setCurrentColor(const RColor& color);
    virtual RColor getCurrentColor();

    void setCurrentLineweight(RLineweight::Lineweight lw);
    RLineweight::Lineweight getCurrentLineweight();

    void setCurrentLinetype(RLinetype lt);
    RLinetype getCurrentLinetype();

    virtual QSharedPointer<RBlock> queryCurrentBlock() {
        return queryBlock(getCurrentBlockId());
    }

    virtual void setCurrentBlock(RBlock::Id blockId) {
        //setVariable("CurrentBlock", blockId);
        currentBlockId = blockId;
    }

    void setCurrentBlock(const QString& blockName) {
        RBlock::Id id = getBlockId(blockName);
        if (id == RBlock::INVALID_ID) {
            return;
        }
        setCurrentBlock(id);
    }

    RBlock::Id getCurrentBlockId() const {
        //return (RBlock::Id) getVariable("CurrentBlock").toInt();
        return currentBlockId;
    }

    void setCurrentView(RView::Id viewId){
        //setVariable("CurrentView", viewId);
        currentViewId = viewId;
    }

    void setCurrentView(const QString& viewName) {
		RView::Id id = getViewId(viewName);
        // unlike blocks and layers, the current view can be invalid:
		setCurrentView(id);
	}

    RView::Id getCurrentViewId() const {
        //return (RView::Id) getVariable("CurrentView").toInt();
        return currentViewId;
    }

    virtual QString getBlockName(RBlock::Id blockId) = 0;
    virtual QSet<QString> getBlockNames() = 0;
    virtual RBlock::Id getBlockId(const QString& blockName) = 0;

    virtual QString getViewName(RView::Id viewId) = 0;
    virtual QSet<QString> getViewNames() = 0;
    virtual RView::Id getViewId(const QString& viewName) = 0;

	virtual QString getLayerName(RLayer::Id layerId) = 0;
	virtual QSet<QString> getLayerNames() = 0;
	virtual RLayer::Id getLayerId(const QString& layerName) = 0;

    virtual QString getLinetypeName(RLinetype::Id linetypeId) = 0;
    virtual QSet<QString> getLinetypeNames() = 0;
    virtual RLinetype::Id getLinetypeId(const QString& linetypeName) = 0;

    virtual void setVariable(const QString& key, const QVariant & value) = 0;
    virtual QVariant getVariable(const QString& key) const = 0;

    /**
     * Clears the selection status of all entities.
     */
    virtual void clearEntitySelection(
        QSet<REntity::Id>* affectedEntities=NULL
    ) = 0;

    /**
     * Selects the entity with the given ID.
     *
     * \param add True to add the entity to the current selection.
     *      False to replace the current selection with the given entity.
     * \param affected Optional pointer to an empty set of entity IDs.
     *      After the call, this set will contain all entity IDs of
     *      entities that were affected by the call.
     */
    virtual void selectEntity(
        REntity::Id entityId,
        bool add=false,
        QSet<REntity::Id>* affectedEntities=NULL
    ) = 0;

    /**
     * Deselects the entity with the given ID.
     *
     * \param affected Optional pointer to an empty set of entity IDs.
     *      After the call, this set will contain all entity IDs of
     *      entities that were affected by the call.
     */
    virtual void deselectEntity(
        REntity::Id entityId,
        QSet<REntity::Id>* affectedEntities=NULL
    ) = 0;

    virtual bool isSelected(REntity::Id entityId) {
        QSharedPointer<REntity> e = queryEntity(entityId);
        return (!e.isNull() && e->isSelected());
    }

    virtual bool isEntity(RObject::Id objectId) {
        QSharedPointer<REntity> e = queryEntity(objectId);
        return !e.isNull();
    }

    /**
     * Selects all entities with the given IDs.
     *
     * \param add True to add the entities to the current selection.
     *      False to replace the current selection with the new entities.
     * \param affected Optional pointer to an empty set of entity IDs.
     *      After the call, this set will contain all entity IDs of
     *      entities that were affected by the call.
     */
    virtual void selectEntities(
        QSet<REntity::Id>& entityIds,
        bool add=false,
        QSet<REntity::Id>* affectedEntities=NULL
    ) = 0;

    void setObjectId(RObject& object, RObject::Id objectId) {
		object.setId(objectId);
	}

    /**
     * \return True if at least one entity is selected in this storage.
     */
    virtual bool hasSelection() const = 0;

    /**
     * \return The bounding box around all visible entities.
     */
    virtual RBox getBoundingBox() = 0;

    virtual RBox getSelectionBox() = 0;

    /**
     * Saves the given object to the storage.
     */
    virtual bool saveObject(QSharedPointer<RObject> object) = 0;

    /**
     * Deletes the object with the given ID.
     */
    virtual void deleteObject(RObject::Id objectId) = 0;

    /**
     * \return The ID of the last completed transaction.
     * The default implementation reads variable "LastTransaction".
     */
    virtual int getLastTransactionId() {
        //return getVariable("LastTransaction").toInt();
        return lastTransactionId;
    }

    /**
     * Sets the ID of the last created transaction. This is called
     * when a transaction is undone or redone (the last transaction
     * id indicates where we are in the transaction stack).
     * The default implementation sets the variable "LastTransaction".
     */
    virtual void setLastTransactionId(int transactionId) {
        //setVariable("LastTransaction", transactionId);
        lastTransactionId = transactionId;
    }

    /**
     * Saves the given transaction. Transactions need to be stored
     * for undo / redo functionality.
     */
    virtual void saveTransaction(RTransaction& transaction) = 0;

    /**
     * Deletes all transactions with IDs from transactionId (inclusive)
     * to the maximum transaction ID. This is called if some transactions
     * have been undone and then a redo is triggerd. All undone
     * transactions have to be deleted at this point.
     */
    virtual void deleteTransactionsFrom(int transactionId) = 0;

    virtual void resetTransactionStack() = 0;

    /**
     * \return Transaction with the given id.
     */
    virtual RTransaction getTransaction(int transactionId) = 0;

    /**
     * Toggles the current undo status of all given objects.
     * The undo status can for example be a boolean value that is true
     * if the object is undone or false otherwise.
     */
    virtual void toggleUndoStatus(QSet<RObject::Id>& objects) = 0;

    /**
     * Toggles the current undo status of the given object.
     */
    virtual void toggleUndoStatus(RObject::Id object) = 0;

    virtual void setUndoStatus(RObject::Id object, bool status) = 0;

    /**
     * \return The highest transaction ID that is in use. This should be
     * used by \ref deleteTransactionsFrom internally.
     */
    virtual int getMaxTransactionId() = 0;

    virtual RLineweight::Lineweight getMaxLineweight() = 0;

	/**
	 * \nonscriptable
	 */
    friend QDebug operator<<(QDebug dbg, RStorage& s);

private:
    RColor currentColor;
    RLineweight::Lineweight currentLineweight;
    RLinetype currentLinetype;
    RView::Id currentViewId;
    RBlock::Id currentBlockId;
    int lastTransactionId;
};

Q_DECLARE_METATYPE(RStorage*)

#endif

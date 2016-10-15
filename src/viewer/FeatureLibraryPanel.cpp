#include "FeatureLibraryPanel.h"
#include "MapFeatureIndex.h"
#include "MapFeatureCollection.h"
#include "FileImportDialog.h"
#include "MapFeature.h"

#include <QFileIconProvider>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>
#include <QUrl>
#include <QClipboard>
#include <QApplication>


using namespace std ;

struct LibraryNode
{
    LibraryNode(const QString &name, quint64 id, bool is_folder, LibraryNode *parent):
        parent_(parent), name_(name), pending_(0), id_(id), is_folder_(is_folder), is_checked_(true) {}

    ~LibraryNode() { qDeleteAll(children_); }

    LibraryNode *parent_;
    QString name_;
    quint64 id_ ;
    QIcon icon_; // cache the icon
    mutable QList<LibraryNode *> children_;
    mutable int pending_;
    mutable bool is_folder_, is_checked_;

    int row() const  {
        if ( parent_ )
            return parent_->children_.indexOf(const_cast<LibraryNode*>(this));
        return 0;
    }

    bool removeChildren(int position, int count) {
        if (position < 0 || position + count > children_.size()) return false;

        for (int row = 0; row < count; ++row)
            delete children_.takeAt(position);

        return true;
    }

    void changeCheckedState(bool state) {
        is_checked_ = state ;

        Q_FOREACH (LibraryNode *child, children_)
            child->changeCheckedState(state) ;

        if ( state )
        {
            LibraryNode *q = parent_ ;
            while ( q ) {
                q->is_checked_ = state ;
                q = q->parent_ ;
            }
        }

    }

    QList<int> path() {
        QList<int> res ;
        LibraryNode *current = this  ;

        while ( current != 0 ) {
            if ( current->parent_ ) res.push_front(current->row()) ;
            current = current->parent_ ;
        }

        return res ;
    }


};

QModelIndex FeatureLibraryModel::fromPath(const QList<int> &path)
{
    LibraryNode *current = root_ ;
    int item ;

    Q_FOREACH(item, path)
        current = current->children_.at(item) ;

    return createIndex(item, 0, current) ;
}

LibraryNode *FeatureLibraryModel::node(const QModelIndex &index) const
{
    LibraryNode  *n = static_cast<LibraryNode *>(index.internalPointer());
    Q_ASSERT(n);
    return n;
}

void FeatureLibraryModel::populate(LibraryNode *parent)
{
    QVector<quint64> ids ;
    QVector<QString> names ;
    QVector<bool> states ;

    feature_index_->getSubFolders(ids, names, states, parent->id_) ;

    for(int i=0 ; i<ids.size() ; i++)
    {
        LibraryNode *node = new LibraryNode(names[i], ids[i], true, parent) ;
        node->is_checked_ = states[i] ;
        parent->children_.append(node) ;

        populate(node) ;
    }

    parent->pending_ = feature_index_->getNumCollections(parent->id_) ;

}

void FeatureLibraryModel::loadPending(LibraryNode *parent)
{
    if ( parent->pending_ == 0 ) return ;

    QVector<quint64> ids ;
    QVector<QString> names ;
    QVector<bool> states ;

    feature_index_->getFolderCollections(ids, names, states, parent->id_) ;

    for(int i=0 ; i<ids.size() ; i++)
    {
        LibraryNode *node = new LibraryNode(names[i], ids[i], false, parent) ;
        node->is_checked_ = states[i] ;
        parent->children_.append(node) ;

    }

    parent->pending_ = 0 ;
}




int FeatureLibraryModel::columnCount(const QModelIndex &parent) const
{
    return 1 ;
}

QModelIndex FeatureLibraryModel::addItem(const QModelIndex &parent, const QString &name, bool folder, bool unique)
{
    if ( !parent.isValid() ) return QModelIndex();

    LibraryNode *parentNode = node(parent) ;

    quint64 parent_folder_id = parentNode->id_ ;
    quint64 item_id ;

    QString unique_name ;

    LibraryNode *item = 0;

    if ( folder )  {
        if ( feature_index_->addNewFolder(name, parent_folder_id, unique_name, item_id) )
            item = new LibraryNode(unique_name, item_id, true, parentNode) ;
    }
    else {
        QMap<QString, QVariant> attr ;
        if ( feature_index_->addNewCollection(name, parent_folder_id, attr, unique_name, item_id) )
            item = new LibraryNode(unique_name, item_id, false, parentNode) ;
    }

    if ( !item ) return QModelIndex() ;

    int row ;

    if ( folder )
    {

        for( row=0; row<parentNode->children_.size() ; row++ )
        {
            if ( !parentNode->children_.at(row)->is_folder_ ) break ;
        }

    }
    else
        row = parentNode->children_.size() ;

    beginInsertRows(parent, row, row+1);
    parentNode->children_.insert(row, item) ;
    endInsertRows();

    return  index(row, 0, parent) ;

}


QModelIndex FeatureLibraryModel::addCollection(const QModelIndex &parent, const CollectionData *col)
{
    if ( !parent.isValid() ) return QModelIndex();

    LibraryNode *parentNode = node(parent) ;

    LibraryNode *item = new LibraryNode(col->name_, col->id_, false, parentNode) ;
    int row = parentNode->children_.size() ;

    beginInsertRows(parent, row, row+1);

    parentNode->children_.insert(row, item) ;

    endInsertRows();

    return  index(row, 0, parent) ;

}

QModelIndex FeatureLibraryModel::addCollectionTree(const QModelIndex &parent, const CollectionTreeNode *col)
{
    if ( !parent.isValid() ) return QModelIndex();

    LibraryNode *parentNode = node(parent) ;

    LibraryNode *item = new LibraryNode(col->name_, col->folder_id_, true, parentNode) ;
    int row = parentNode->children_.size() ;

    if ( col->collection_ )
    {
        LibraryNode *child = new LibraryNode(col->collection_->name_, col->collection_->id_, false, item) ;
        item->children_.push_back(child) ;
    }

    beginInsertRows(parent, row, row+1);

    parentNode->children_.insert(row, item) ;

    endInsertRows();

    QModelIndex idx = index(row, 0, parent) ;

    Q_FOREACH(CollectionTreeNode *col, col->children_)
        addCollectionTree(idx, col) ;

    return idx ;

}

QModelIndex FeatureLibraryModel::moveItem(const QModelIndex &from, const QModelIndex &to, const QString &name, quint64 id, bool folder)
{
    if ( !to.isValid() ) return QModelIndex();

    LibraryNode *parentNode = node(to) ;
    loadPending(parentNode) ;

    quint64 parent_folder_id = parentNode->id_ ;
    quint64 item_id ;

    QString unique_name ;

    LibraryNode *item = 0;

    if ( folder )  {
        if ( feature_index_->moveFolder(id, parent_folder_id) )
            item = new LibraryNode(name, id, true, parentNode) ;
    }
    else {
        if ( feature_index_->moveCollection(id, parent_folder_id) )
            item = new LibraryNode(name, id, false, parentNode) ;
    }

    if ( !item ) return QModelIndex() ;

    int row ;

    if ( folder )
    {
        for( row=0; row<parentNode->children_.size() ; row++ )
        {
            if ( !parentNode->children_.at(row)->is_folder_ ) break ;
        }

    }
    else
        row = parentNode->children_.size() ;

    beginInsertRows(to, row, row+1);
    parentNode->children_.insert(row, item) ;
    endInsertRows();

    QModelIndex from_parent = from.parent() ;
    parentNode = node(from_parent) ;

    beginRemoveRows(from_parent, from.row(), from.row());
    parentNode->removeChildren(from.row(), 1);
    endRemoveRows();

    return  index(row, 0, to) ;

}

bool FeatureLibraryModel::deleteItem(const QModelIndex &index)
{
    LibraryNode * item = node(index);
    LibraryNode * parentItem = item->parent_ ;

    if ( parentItem == 0 ) return false ;

    int position = item->row() ;

    bool success ;

    beginRemoveRows(index.parent(), position, position);

    success = parentItem->removeChildren(position, 1);

    if ( item->is_folder_ )
        feature_index_->deleteFolder(item->id_) ;
    else
        feature_index_->deleteCollection(item->id_) ;

    endRemoveRows();

    return success ;
}


QVariant FeatureLibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    LibraryNode *item = node(index) ;

    if ( role == Qt::DecorationRole)
    {
        if (  item->is_folder_ ) return icon_provider_->icon(QFileIconProvider::Folder);
        else return icon_provider_->icon(QFileIconProvider::File);
    }
    else if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        return item->name_;
    }
    else if ( role == Qt::CheckStateRole || role == Qt::UserRole + 3)
    {
        return ( item->is_checked_ ) ? Qt::Checked : Qt::Unchecked ;
    }
    else if ( role == Qt::UserRole + 1 )
    {
        if ( item->is_folder_ ) return item->id_ ;
        else return -1 ;
    }
    else if ( role == Qt::UserRole + 2 )
    {
        if ( !item->is_folder_ ) return item->id_ ;
        else return -1 ;
    }
    else
        return QVariant();

}

bool FeatureLibraryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    LibraryNode *item = node(index);
    QString newName = value.toString() ;

    if ( role == Qt::CheckStateRole ) {

        item->changeCheckedState( value == Qt::Checked ) ;

        if ( !item->is_folder_ )
            feature_index_->setCollectionVisibility(item->id_, value == Qt::Checked) ;
        else
            feature_index_->setFolderVisibility(item->id_, value == Qt::Checked) ;

        emit dataChanged(this->index(0, 0), this->index(item->children_.size(), index.column(), index));

        return true ;
    }
    else if ( role == Qt::EditRole ) {
        if ( item->is_folder_ && !feature_index_->renameFolder(item->id_, newName) ) return false ;
        else if ( !item->is_folder_ && !feature_index_->renameCollection(item->id_, newName) ) return false ;
        else {
            item->name_ = newName ;
            emit dataChanged(index, index);
            return true;
        }
    }
    else if ( role == Qt::UserRole+3 )
    {
        item->is_checked_ = ( value == Qt::Checked ) ;
        emit dataChanged(this->index(0, 0), this->index(item->children_.size(), index.column(), index));
    }
    else return false ;
}

Qt::DropActions FeatureLibraryModel::supportedDropActions() const{
    return Qt::MoveAction | Qt::CopyAction ;
}

QStringList FeatureLibraryModel::mimeTypes() const
{
    QStringList types;
    types << "application/x-node-list";
    types << "text/uri-list";
    return types;
}

QMimeData *FeatureLibraryModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            LibraryNode *item = node(index) ;
            stream << item->name_ << item->id_ << item->is_folder_ << item->path() ;
        }
    }

    mimeData->setData("application/x-node-list", encodedData);
    return mimeData;
}

bool FeatureLibraryModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                       int row, int column, const QModelIndex &parent)
{
    if ( action == Qt::IgnoreAction ) return true;

    if ( column > 0 ) return false;

    if ( data->hasFormat("application/x-node-list") )
    {
        QByteArray encodedData = data->data("application/x-node-list");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        while (!stream.atEnd()) {
            QString name ;
            quint64 id ;
            bool is_folder ;
            QList<int> path ;

            stream >> name >> id >> is_folder >> path ;

            QModelIndex from = fromPath(path) ;

            //      if ( action == Qt::MoveAction )
            moveItem(from, parent, name, id, is_folder) ;
        }

        return true;
    }
    else if ( data->hasFormat("text/uri-list") )
    {
        LibraryNode *parentNode = node(parent) ;

        QList<QUrl> urls = data->urls();

        QStringList file_list ;
        for (int i = 0; i < urls.size(); ++i)
        {
            QString file_name = urls[i].toLocalFile();

            if ( !file_name.isEmpty() )
                file_list.append(file_name) ;
        }

        FileImportDialog dlg(file_list, parentNode->id_, feature_index_, 0) ;
        dlg.exec() ;

        for(int i=0 ; i<dlg.collections_.size() ; i++ )
        {
            QModelIndex item = addCollection(parent, dlg.collections_[i]) ;
        }

        qDeleteAll(dlg.collections_) ;


    }

    return false ;
}

Qt::ItemFlags FeatureLibraryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;

    LibraryNode *item = node(index);

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable;
    if ( item->is_folder_ ) flags |= Qt::ItemIsDropEnabled ;
    return flags ;
}


QModelIndex FeatureLibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    if ( !hasIndex(row, column, parent) ) return QModelIndex();

    LibraryNode *parentItem;

    if (!parent.isValid())
        parentItem = root_;
    else
        parentItem = node(parent) ;

    LibraryNode *childItem = parentItem->children_.at(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex FeatureLibraryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    LibraryNode *childItem = node(index) ;
    LibraryNode *parentItem = childItem->parent_ ;

    if (parentItem == root_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int FeatureLibraryModel::rowCount(const QModelIndex &parent) const
{
    LibraryNode *parentItem;

    if (parent.column() > 0) return 0;

    if (!parent.isValid())
        parentItem = root_;
    else
        parentItem = node(parent) ;

    return parentItem->children_.size();
}

bool FeatureLibraryModel::hasChildren ( const QModelIndex & parent ) const
{
    LibraryNode *parentItem = ( !parent.isValid() ) ? root_ : node(parent) ;

    if ( parentItem->is_folder_)
        return parentItem->children_.size() + parentItem->pending_ ;
    else
        return false ;
}


void FeatureLibraryModel::fetchMore(const QModelIndex & parent)
{
    LibraryNode *parentItem = ( !parent.isValid() ) ? root_ : node(parent) ;

    if ( !parentItem->is_folder_ ) return ;

    loadPending(parentItem) ;
}



bool FeatureLibraryModel::canFetchMore ( const QModelIndex & parent ) const
{
    LibraryNode *parentItem = ( !parent.isValid() ) ? root_ : node(parent) ;

    if ( !parentItem->is_folder_ ) return false ;

    return parentItem->pending_ ;
}


FeatureLibraryModel::FeatureLibraryModel(MapFeatureIndex *index): feature_index_(index)
{

    icon_provider_ = new QFileIconProvider() ;
    root_ = new LibraryNode("root", 0, true, 0) ;

    populate(root_) ;
}

FeatureLibraryModel::~FeatureLibraryModel()
{
    delete root_ ;
    delete icon_provider_ ;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// preorder traversal of tree model

void FeatureLibraryView::traverseModel(const QModelIndex &parent, QModelIndexList &state)
{
    LibraryNode *p = model_.node(parent) ;

    if ( !p->is_folder_) return ;

    state.append(parent) ;

    for(int i=0 ; i<model_.rowCount(parent) ; i++)
    {
        traverseModel(parent.child(i, 0), state) ;
    }
}

void FeatureLibraryView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList idxs = selected.indexes() ;
    if ( idxs.empty() ) return ;

    QModelIndex index = idxs.at(0) ;

    if ( !index.isValid() ) return ;

    LibraryNode *item = model_.node(index);

    updateContextMenu() ;

    if ( item->is_folder_ ) {
        emit folderClicked(item->id_) ;
    } else {
        emit collectionClicked(item->id_, -1) ;
    }

}

void FeatureLibraryView::updateContextMenu()
{
    bool is_folder_selected = false ;

    QModelIndexList idxs = selectionModel()->selectedIndexes() ;

    if ( !idxs.isEmpty() )
    {
        LibraryNode *item = model_.node(idxs.at(0)) ;
        is_folder_selected = item->is_folder_ ;
    }

    new_folder_action_->setEnabled(is_folder_selected) ;
    new_collection_action_->setEnabled(is_folder_selected) ;

    zoom_action_->setDisabled(is_folder_selected) ;

    QClipboard *clipboard = QApplication::clipboard() ;
    const QMimeData *data = clipboard->mimeData() ;
    if ( data->hasFormat("application/x-feature-list") )
        paste_action_->setDisabled(is_folder_selected ) ;

}

void FeatureLibraryView::onPaste()
{
    QClipboard *clipboard = QApplication::clipboard() ;
    const QMimeData *data = clipboard->mimeData() ;

    QByteArray encodedData = data->data("application/x-feature-list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    QVector<quint64> ids ;
    stream >> ids ;

    QModelIndex index = currentIndex() ;

    if ( !index.isValid() )  return ;

    LibraryNode *node = model_.node(index) ;

    feature_index_->addFeaturesInCollection(node->id_, ids) ;

    emit collectionClicked(node->id_, -1) ;
}

bool FeatureLibraryView::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if ( keyEvent->key() == Qt::Key_Delete )
        {
            deleteItem() ;
            return true ;
        }
        else
            return false ;
    } else {
        // standard event processing
        return QTreeView::eventFilter(obj, event);
    }

}


void FeatureLibraryView::deleteItem()
{
    QModelIndex index = selectionModel()->currentIndex() ;

    if ( !index.isValid() )  return ;

    LibraryNode *node = model_.node(index) ;

    if ( node->is_folder_ ) {
        QMessageBox msgBox;
        msgBox.setText(QString("Do you really want to delete folder \"%1\" and its contents ?").arg(node->name_));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);

        if ( msgBox.exec() == QMessageBox::Yes )
            model_.deleteItem(index) ;
    }
    else
        model_.deleteItem(index) ;

}

FeatureLibraryView::FeatureLibraryView(MapFeatureIndex *index, QWidget *parent): QTreeView(parent), model_(index), feature_index_(index)
{
    setModel(&model_) ;
    setHeaderHidden(true);
    resizeColumnToContents(0) ;
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);

    context_menu_ = new QMenu(this);

    zoom_action_ = new QAction( tr("Center on map"), this);
    connect(zoom_action_, SIGNAL(triggered()), this, SLOT(onZoom()));
    zoom_action_->setEnabled(false) ;

    paste_action_ = new QAction( tr("Paste"), this);
    connect(paste_action_, SIGNAL(triggered()), this, SLOT(onPaste()));
    paste_action_->setEnabled(false) ;

    new_folder_action_ = new QAction( tr("New folder"), this);
    connect(new_folder_action_, SIGNAL(triggered()), this, SLOT(addNewFolder()));
    new_folder_action_->setEnabled(false) ;

    new_collection_action_ = new QAction( tr("New feature collection"), this);
    connect(new_collection_action_, SIGNAL(triggered()), this, SLOT(addNewCollection()));
    new_collection_action_->setEnabled(false) ;

    delete_item_action_ = new QAction( tr("Delete"), this);
    connect(delete_item_action_, SIGNAL(triggered()), this, SLOT(deleteItem()));

    context_menu_->addAction(zoom_action_) ;
    context_menu_->addSeparator() ;
    context_menu_->addAction(paste_action_) ;
    context_menu_->addAction(new_folder_action_) ;
    context_menu_->addAction(new_collection_action_) ;
    context_menu_->addSeparator() ;
    context_menu_->addAction(delete_item_action_) ;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onContextMenu(const QPoint &)));

    connect(selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,SLOT( onSelectionChanged(const QItemSelection&, const QItemSelection & ) ) );

    installEventFilter(this) ;


}

void FeatureLibraryView::addNewFolder()
{
    QModelIndex index = currentIndex();
    if ( !index.isValid() )  return ;

    LibraryNode *parentNode = model_.node(index) ;
    if ( !parentNode->is_folder_ ) index = index.parent() ;

    QModelIndex item = model_.addItem(index, tr("New Folder"), true) ;

    if ( item.isValid() )
    {
        setExpanded(index, true);
        QItemSelectionModel *selection = selectionModel() ;
        selection->clearSelection();
        selection->select(item, QItemSelectionModel::Select | QItemSelectionModel::Current| QItemSelectionModel::Rows ) ;
        edit(item) ;

    }
}

void FeatureLibraryView::addNewCollection()
{
    QModelIndex index = currentIndex();
    if ( !index.isValid() )  return ;

    LibraryNode *parentNode = model_.node(index) ;
    if ( !parentNode->is_folder_ ) index = index.parent() ;

    QModelIndex item = model_.addItem(index, tr("New Collection"), false) ;

    if ( item.isValid() )
    {
        setExpanded(index, true);
        QItemSelectionModel *selection = selectionModel() ;
        selection->clearSelection();
        selection->select(item, QItemSelectionModel::Select | QItemSelectionModel::Current | QItemSelectionModel::Rows) ;
        edit(item) ;
    }
}

void FeatureLibraryView::addCollection(const CollectionData *col)
{
    QModelIndex index = currentIndex();
    if ( !index.isValid() )  return ;

    QModelIndex item = model_.addCollection(index, col) ;

    if ( item.isValid() )
    {
        setExpanded(index, true);
        QItemSelectionModel *selection = selectionModel() ;
        selection->clearSelection();
        selection->select(item, QItemSelectionModel::Select | QItemSelectionModel::Current | QItemSelectionModel::Rows) ;
    }
}

void FeatureLibraryView::addCollectionTree(const CollectionTreeNode *col)
{
    QModelIndex index = currentIndex();
    if ( !index.isValid() )  return ;

    QModelIndex item = model_.addCollectionTree(index, col) ;

    if ( item.isValid() )
    {
        setExpanded(index, true);
        QItemSelectionModel *selection = selectionModel() ;
        selection->clearSelection();
        selection->select(item, QItemSelectionModel::Select | QItemSelectionModel::Current | QItemSelectionModel::Rows) ;
    }
}

QByteArray FeatureLibraryView::saveState()
{
    QByteArray data ;
    QDataStream stream(&data, QIODevice::WriteOnly) ;

    QModelIndexList idxs ;
    traverseModel(model_.index(0, 0), idxs) ;

    Q_FOREACH(QModelIndex idx, idxs)
    {
        stream << isExpanded(idx)  ;
    }

    return data ;
}

void FeatureLibraryView::restoreState(const QByteArray &data)
{
    if ( data.isEmpty() ) return ;

    QDataStream stream(data) ;

    QModelIndexList idxs ;
    traverseModel(model_.index(0, 0), idxs) ;

    Q_FOREACH(QModelIndex idx, idxs)
    {
        bool is_expanded ;
        int state ;
        stream >> is_expanded ;

        if ( is_expanded ) setExpanded(idx, true) ;
    }

}


void FeatureLibraryView::onContextMenu(const QPoint &)
{
    context_menu_->exec(QCursor::pos()) ;
}

void FeatureLibraryView::onZoom()
{
    QModelIndex index = currentIndex();
    if ( !index.isValid() )  return ;

    LibraryNode *item = model_.node(index) ;

    QRectF bbox = feature_index_->getCollectionBBox(item->id_) ;

    emit zoomOnRect(bbox) ;
}

void FeatureLibraryView::expandFolder(quint64 folder_id)
{
    QModelIndexList items = model_.match(model_.index(0, 0), Qt::UserRole + 1, QVariant(folder_id), 1, Qt::MatchExactly | Qt::MatchRecursive) ;

    QModelIndex item = items.at(0) ;

    while ( item.isValid() ) {
        expand(item) ;
        item = item.parent() ;
    }


}

void FeatureLibraryView::selectCollection(quint64 collection_id)
{
    QModelIndexList items = model_.match(model_.index(0, 0), Qt::UserRole + 2, QVariant(collection_id), 1, Qt::MatchExactly | Qt::MatchRecursive) ;

    Q_FOREACH( const QModelIndex &item, items) {
        selectionModel()->select(item, QItemSelectionModel::ToggleCurrent);
    }

}


void FeatureLibraryView::onFeatureClicked(quint64 id)
{
    quint64 collection_id, folder_id ;
    if ( feature_index_->getFeatureCollectionAndFolder(id, collection_id, folder_id) )
    {
        expandFolder(folder_id) ;
        emit collectionClicked(collection_id, id);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FeatureListView::FeatureListView(MapFeatureIndex *index, QWidget *parent): QListView(parent), feature_index_(index)
{
    setModel(&model_) ;
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);

    context_menu_ = new QMenu(this);

    zoom_action_ = new QAction( tr("Center on map"), this);
    connect(zoom_action_, SIGNAL(triggered()), this, SLOT(onZoom()));
    zoom_action_->setEnabled(false) ;

    copy_action_ = new QAction( tr("Copy"), this);
    connect(copy_action_, SIGNAL(triggered()), this, SLOT(onCopy()));
    copy_action_->setEnabled(false) ;

    cut_action_ = new QAction( tr("Cut"), this);
    connect(cut_action_, SIGNAL(triggered()), this, SLOT(onCut()));
    cut_action_->setEnabled(false) ;

    duplicate_action_ = new QAction( tr("Duplicate"), this);
    connect(duplicate_action_, SIGNAL(triggered()), this, SLOT(onDuplicate()));
    duplicate_action_->setEnabled(false) ;

    delete_item_action_ = new QAction( tr("Delete"), this);
    connect(delete_item_action_, SIGNAL(triggered()), this, SLOT(deleteItem()));

    context_menu_->addAction(zoom_action_) ;
    context_menu_->addSeparator() ;
    context_menu_->addAction(copy_action_) ;
    context_menu_->addAction(cut_action_) ;
    context_menu_->addAction(duplicate_action_) ;
    context_menu_->addSeparator() ;
    context_menu_->addAction(delete_item_action_) ;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onContextMenu(const QPoint &)));

    connect(selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,SLOT( onSelectionChanged(const QItemSelection&, const QItemSelection & ) ) );

    installEventFilter(this) ;

    setStyleSheet(
        "QTreeView::indicator:unchecked {image: url(:/icons/eye_grey.png);}"
        "QTreeView::indicator:checked {image: url(:/icons/eye.png);}"
      );

}

bool FeatureListView::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if ( keyEvent->key() == Qt::Key_Delete )
        {
            deleteItem() ;
            return true ;
        }
        else
            return false ;
    } else {
        // standard event processing
        return QListView::eventFilter(obj, event);
    }

}

void FeatureListView::populate(quint64 collection_id, quint64 feature_id)
{
    collection_id_ = collection_id ;

    model_.clear() ;

    QVector<MapFeaturePtr> features ;

    feature_index_->getAllFeaturesInCollection(collection_id, features) ;

    Q_FOREACH(MapFeaturePtr feature, features)
    {
        QStandardItem *item = new QStandardItem() ;

        item->setText(feature->name()) ;
        if ( feature->type() == "polygon" ) item->setIcon(QIcon(":/images/polygon-tool.png")) ;
        else if ( feature->type() == "marker" ) item->setIcon(QIcon(":/images/flag-blue.png")) ;
        item->setData(feature->id()) ;
        item->setData(feature->type(), Qt::UserRole+2) ;

        model_.appendRow(item) ;
    }

    QModelIndexList items = model_.match(model_.index(0, 0), Qt::UserRole+1, QVariant(feature_id)) ;

    if ( !items.isEmpty() )
    {
        QModelIndex item = items.at(0) ;
        selectionModel()->select(item, QItemSelectionModel::SelectCurrent) ;
    }

}

void FeatureListView::getSelected(const QModelIndexList &selected, QVector<quint64> &ids)
{

    Q_FOREACH(const QModelIndex &index, selected)
    {
        quint64 id = index.data(Qt::UserRole+1).toULongLong() ;
        ids.append(id) ;
    }
}

void FeatureListView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    updateContextMenu() ;

    QVector<quint64> features ;

    QModelIndexList idxs = selectionModel()->selectedIndexes();

    getSelected(idxs, features) ;

    emit featuresSelected(features) ;

}

void FeatureListView::updateContextMenu()
{
    QModelIndexList idxs = selectionModel()->selectedIndexes() ;

    zoom_action_->setDisabled(idxs.isEmpty()) ;
    delete_item_action_->setDisabled(idxs.isEmpty()) ;

    copy_action_->setDisabled(idxs.isEmpty()) ;
    cut_action_->setDisabled(idxs.isEmpty()) ;
    duplicate_action_->setDisabled(idxs.isEmpty()) ;
}

void FeatureListView::onZoom()
{
    QVector<quint64> ids ;
    getSelected(selectionModel()->selectedIndexes(), ids) ;

    QRectF box = feature_index_->getFeatureBBox(ids) ;

    emit zoomOnRect(box) ;
}

void FeatureListView::deleteItem()
{
    QModelIndexList indices = selectionModel()->selectedIndexes() ;

    QVector<quint64> ids ;
    getSelected(indices, ids) ;

    feature_index_->deleteFeaturesFromCollection(collection_id_, ids) ;

    QList<int> rows;
    Q_FOREACH( const QModelIndex & index, indices ) {
        rows.append( index.row() );
    }

    qSort( rows );

    int prev = -1;
    for( int i = rows.count() - 1; i >= 0; i -= 1 ) {
        int current = rows[i];
        if( current != prev ) {
            model_.removeRow(current);
            prev = current;
        }
    }

}

void FeatureListView::onCopy()
{
    QClipboard *clipboard = QApplication::clipboard();

    QVector<quint64> ids ;
    getSelected(selectionModel()->selectedIndexes(), ids) ;

    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    stream << ids ;

    mimeData->setData("application/x-feature-list", encodedData);

    clipboard->setMimeData(mimeData) ;
}

void FeatureListView::onCut()
{
    onCopy() ;
    deleteItem() ;
}

void FeatureListView::onDuplicate()
{
    QVector<quint64> ids ;
    getSelected(selectionModel()->selectedIndexes(), ids) ;

    QVector<MapFeaturePtr> features ;
    feature_index_->getAllFeatures(ids, features) ;

    Q_FOREACH(MapFeaturePtr feature, features)
    {
        int counter = 1 ;

        QString name = feature->name() ;

        QString unique_name = feature_index_->uniqueFeatureName(name + " (%1)", collection_id_, counter) ;

        feature->setName(unique_name) ;
    }

    feature_index_->write(features, collection_id_) ;

    populate(collection_id_) ;


}


void FeatureListView::onContextMenu(const QPoint &)
{
    context_menu_->exec(QCursor::pos()) ;
}


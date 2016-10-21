#ifndef __FEATURE_LIBRARY_PANEL_H__
#define __FEATURE_LIBRARY_PANEL_H__

#include <QStandardItemModel>
#include <QSplitter>
#include <QTreeView>
#include <QListView>
#include <QLabel>
#include <QPixmap>

#include "map_overlay_manager.hpp"
#include "overlay_import.hpp"

class MainWindow ;
class MapFeatureIndex ;
class MapFeatureCollection ;
class QFileIconProvider ;

struct LibraryNode ;


class FeatureLibraryModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    FeatureLibraryModel(QSharedPointer<MapOverlayManager> m) ;
    ~FeatureLibraryModel() ;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column,  const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
 //   QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags) const ;

    QModelIndex fromPath(const QList<int> &path);

    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,  int row, int column, const QModelIndex &parent);
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;


    QModelIndex addItem( const QModelIndex &parent, const QString &name, bool is_folder_, bool unique = true) ;
    bool deleteItem(const QModelIndex &index) ;

    QModelIndex addCollection(const QModelIndex &parent, const CollectionData *col);
    QModelIndex moveItem(const QModelIndex &from, const QModelIndex &to, const QString &name, quint64 id, bool folder);

    QModelIndex addCollectionTree(const QModelIndex &parent, const CollectionTreeNode *col);

private:
    friend class FeatureLibraryView ;

    inline bool indexValid(const QModelIndex &index) const {
        return (index.row() >= 0) && (index.column() >= 0) && (index.model() == this);
    }

    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const ;
    bool canFetchMore ( const QModelIndex & parent = QModelIndex() ) const ;
    void fetchMore(const QModelIndex & parent) ;

    LibraryNode *node(const QModelIndex &index) const ;
    void populate(LibraryNode *parent) ;
    void loadPending(LibraryNode *parent) ;

    void traverseState(const QModelIndex &parent) ;

Q_SIGNALS:
    void collectionVisibilityChanged() ;

private:

    QSharedPointer<MapOverlayManager> overlay_manager_ ;
    mutable LibraryNode *root_;
    QFileIconProvider *icon_provider_ ;
} ;

class FeatureLibraryView: public QTreeView {
    Q_OBJECT

public:

    FeatureLibraryView(QSharedPointer<MapOverlayManager> mgr, QWidget *parent) ;

    void addCollection(const CollectionData *col);
    void addCollectionTree(const CollectionTreeNode *col);

    void expandFolder(quint64 folder_id) ;
    void selectCollection(quint64 collection_id) ;

    QByteArray saveState() ;
    void restoreState(const QByteArray &data) ;

protected:
    bool eventFilter(QObject *obj, QEvent *event) ;
Q_SIGNALS:

    void collectionClicked(quint64, quint64) ;
    void folderClicked(quint64) ;
    void zoomOnRect(QRectF) ;

private Q_SLOTS:
    void addNewFolder() ;
    void addNewCollection() ;
    void onContextMenu(const QPoint &) ;
    void onZoom() ;
    void deleteItem();
    void updateContextMenu() ;
    void onPaste() ;
    void onFeatureClicked(quint64 id) ;

    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void traverseModel(const QModelIndex &parent, QModelIndexList &state);
private:

    FeatureLibraryModel model_;
    QMenu *context_menu_ ;
    QSharedPointer<MapOverlayManager> overlay_manager_ ;
    QAction *zoom_action_, *paste_action_, *new_folder_action_, *new_collection_action_, *delete_item_action_ ;

} ;


class FeatureListView: public QListView {
    Q_OBJECT

public:

    FeatureListView(QSharedPointer<MapOverlayManager> mgr, QWidget *parent) ;

protected:
    bool eventFilter(QObject *obj, QEvent *event) ;
    void getSelected(const QModelIndexList &selected, QVector<quint64> &ids);
signals:

    void featuresSelected(QVector<quint64>) ;

public Q_SLOTS:
    void populate(quint64 collection_id, quint64 selection_id = 0) ;
    void clear() ;

private Q_SLOTS:
    void onContextMenu(const QPoint &) ;
    void updateContextMenu() ;
    void onZoom() ;
    void deleteItem() ;
    void onCopy() ;
    void onCut() ;
    void onDuplicate() ;


    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
Q_SIGNALS:
    void zoomOnRect(QRectF) ;
private:

    quint64 collection_id_ ;
    QSharedPointer<MapOverlayManager> overlay_manager_ ;
    QStandardItemModel model_;
    QMenu *context_menu_ ;
    QAction *zoom_action_, *copy_action_, *cut_action_, *duplicate_action_, *delete_item_action_ ;
} ;


#endif

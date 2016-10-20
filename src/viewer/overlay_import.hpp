#ifndef __OVERLAY_IMPORT_HPP__
#define __OVERLAY_IMPORT_HPP__


#include <QMap>
#include <QVariant>

#include "map_overlay_manager.hpp"

struct CollectionData
{
    QString name_ ;                     // name of the collection
    quint64 folder_ ;                     // path of the collection in the library
    QMap<QString, QVariant> attributes_ ;   // attributes associated with the collection
    quint64 id_ ;                   // database id
} ;

struct CollectionTreeNode
{
    CollectionTreeNode(): parent_(0), collection_(0) {}

    QString name_ ;
    QVector<CollectionTreeNode *> children_ ;
    quint64 folder_id_ ;
    CollectionTreeNode *parent_ ;
    CollectionData * collection_ ;
    QVector<MapOverlayPtr> overlay_list_ ;

    ~CollectionTreeNode() {
        if ( collection_ ) delete collection_ ;
        Q_FOREACH(CollectionTreeNode *col, children_) delete col ;
    }
};

class OverlayReader {
public:
    virtual QString filters() const = 0;
    virtual QString description() const = 0 ;
    virtual struct CollectionTreeNode *import(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx) = 0 ;
} ;

class OverlayImportManager {
public:

    CollectionTreeNode *read(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx)  ;

    void registerReader(OverlayReader *f) {
        readers_.append(f) ;
    }

    static OverlayImportManager &instance() {

        static OverlayImportManager instance_ ;
        return instance_ ;
    }

    QString filter() const ;

    QVector<OverlayReader *> readers_ ;
};


#endif

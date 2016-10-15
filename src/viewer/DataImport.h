#ifndef __DATA_IMPORT_H__
#define __DATA_IMPORT_H__

#include <QMap>
#include <QVariant>

#include "MapFeatureIndex.h"
#include "MapFeature.h"

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
    QVector<MapFeaturePtr> feature_list_ ;

    ~CollectionTreeNode() {
        if ( collection_ ) delete collection_ ;
        Q_FOREACH(CollectionTreeNode *col, children_) delete col ;
    }
};

CollectionData *importGpx(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx) ;
CollectionTreeNode *importKmz(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx) ;
CollectionTreeNode *importKml(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx) ;
CollectionTreeNode *importKml(QIODevice *data, quint64 folder_id, MapFeatureIndex *fidx) ;

#endif

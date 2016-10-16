#ifndef __MAP_FEATURE_COLLECTION_H__
#define __MAP_FEATURE_COLLECTION_H__

#include <QRect>
#include <QMap>
#include <QVector>
#include <QVariant>


class MapFeature ;
class MapFeatureIndex ;

// Several features may be grouped together into a collection
// Only features id's are kept in memory

class MapFeatureCollection
{
public:
    MapFeatureCollection(MapFeatureIndex *fidx) ;

    // These will add/remove a feature from the collection and update the database
    bool addFeature(MapFeature *f) ;
    bool removeFeature(MapFeature *f) ;

    bool addFeatures(const QVector<quint64> &features) ;

    quint64 id() const { return storage_id_ ; }
    QRectF boundingBox() const { return box_ ; }

    QString name() const { return name_ ; }

    static MapFeatureCollection *importGpx(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx) ;

protected:

    friend class KmlDocumentNode ;

    QString name_ ;                     // name of the collection
    quint64 path_ ;                     // path of the collection in the library
    QVector<int> features_ ;            // feature id's participating in the collection
    QMap<QString, QVariant> attributes_ ;   // attributes associated with the collection
    quint64 storage_id_ ;                   // database id
    QRectF box_ ;                           // bounding box of all features (to be able to zoom on them)
    MapFeatureIndex *feature_index_ ;       // pointer to the database

private:

    bool write(quint64 folderId) ;
};





class KmlDocumentNode
{
public:

    KmlDocumentNode(): parent_(0) {}

    static KmlDocumentNode *importKmz(const QString &fileName, quint64 parent_folder_id, MapFeatureIndex *fidx) ;
    static KmlDocumentNode *importKml(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx) ;
    static KmlDocumentNode *importKml(QIODevice *data, quint64 folder_id, MapFeatureIndex *fidx) ;
    static void createFoldersRecursive(KmlDocumentNode *node, quint64 parent_id, MapFeatureIndex *fidx) ;

private:
    friend class KmlHandler ;

    QString name_ ;
    QVector<KmlDocumentNode *> children_ ;
    QVector<MapFeature *> feature_list_ ;
    quint64 folder_id_ ;
    KmlDocumentNode *parent_ ;
    MapFeatureCollection *collection_ ;
};




#endif

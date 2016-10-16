#ifndef __OVERLAY_INDEX_H__
#define __OVERLAY_INDEX_H__

#include <QObject>
#include <QPolygonF>
#include <QMap>

#include "database.hpp"
#include "map_overlay.hpp"

namespace SpatialIndex {
    class ISpatialIndex ;
    class IStorageManager ;

    namespace StorageManager {
        class IBuffer ;
    }
}

class MapFeature ;
class MapWidget ;

class MapFeatureIndex: public QObject {

    Q_OBJECT

public:

    MapFeatureIndex(QObject *parent) ;
    ~MapFeatureIndex();

    // open or create spatial index with given prefix
    bool open(const QString &storage) ;


    // find feature under click coordinates
    MapFeaturePtr findNearest(const QByteArray &searchType, const QPoint &coord, MapWidget *view, double thresh);

    // load and create an object with this id from database
    MapFeaturePtr load(quint64 id) ;

    // update feature
    bool update(const MapFeaturePtr &feature) ;

    // write object(s) to database and spatial index
    bool write(const QVector<MapFeaturePtr> &objects, quint64 collection_id = 0) ;

    // query objects within the collection of rectangles
    void query(QVector<quint64> &ovr, QVector<QRectF> &boxes) ;

    bool addNewFolder(const QString &name, quint64 parent_id, QString &unique_name, quint64 &item_id) ;
    bool addNewCollection(const QString &name, quint64 parent_id, const QMap<QString, QVariant> &attributes, QString &unique_name, quint64 &item_id) ;

    bool addFeaturesInCollection(quint64 collection_id, const QVector<quint64> &features) ;
    bool deleteFeaturesFromCollection(quint64 collection_id, const QVector<quint64> &features) ;

    bool getSubFolders(QVector<quint64> &ids, QVector<QString> &names, QVector<bool> &state, quint64 parent_id);
    bool getFolderCollections(QVector<quint64> &ids, QVector<QString> &names, QVector<bool> &states, quint64 folder_id);
    int getNumCollections(quint64 folder_id);
    bool getFeatureCollectionAndFolder(quint64 feature_id, quint64 &collection_id, quint64 &folder_id) ;

    bool getAllFeaturesInCollection(quint64 collection_id, QVector<MapFeaturePtr> &features) ;
    bool getAllFeatures(const QVector<quint64> feature_ids, QVector<MapFeaturePtr> &features);
    bool getAllFeaturesInFolder(quint64 folder_id, QVector<MapFeaturePtr> &features) ;

    bool deleteFolder(quint64 id) ;
    bool deleteCollection(quint64 id) ;

    bool renameFolder(quint64 id, const QString &newName) ;
    bool renameCollection(quint64 id, const QString &newName) ;

    bool moveFolder(quint64 folder_id, quint64 parent_folder_id) ;
    bool moveCollection(quint64 collection_id, quint64 parent_folder_id) ;

    bool setCollectionVisibility(quint64 collection_id, bool state) ;

    // get a unique feature name for a collection using the specified arg pattern. counter should contain the initial numeric value and is update after the call

    QString uniqueFeatureName(const QString &pattern, quint64 collection_id, int &counter);

    QRectF getFeatureBBox(const QVector<quint64> &feature_ids) ;
    QRectF getCollectionBBox(quint64 collection_id);
    QRectF getFolderBBox(quint64 folder_id);

    bool setFolderVisibility(quint64 id, bool state, bool update_children = true);

protected:

    bool write(const QByteArray &data, const QByteArray &type, const QRectF &mbr, quint64 collection_id, quint64 &id);

    friend class MapFeatureCollection ;

    SQLite::Database *db() const { return db_ ; }

private:

    SpatialIndex::ISpatialIndex *index_ ;           // R-Tree index
    SpatialIndex::IStorageManager* storage_ ;       // data storage (only id's are stored here)
    SpatialIndex::StorageManager::IBuffer *buffer_ ;// memory buffer containing loaded pages
    SQLite::Database *db_ ;                         // the actual data are serialized here

    static const size_t index_page_size_ ;
    static const size_t index_buffer_capacity_ ;
};


#endif

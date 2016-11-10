#include "map_overlay_manager.hpp"
#include "map_overlay.hpp"
#include "map_widget.hpp"

#include "spatialindex/SpatialIndex.h"


#include <QFileInfo>
#include <QDataStream>
#include <QDebug>
#include <QSettings>

#include <float.h>

#include "database.hpp"
#include "tms.hpp"
#include "overlay_import.hpp"

#include <spatialite.h>

using namespace SpatialIndex ;
using namespace std ;

const size_t MapOverlayManager::index_page_size_ = 64 * 1024 ;
const size_t MapOverlayManager::index_buffer_capacity_ = 16 * 1024 * 1024 ;

bool MapOverlayManager::open(const QString &storage)
{
    // this is the sqlite database that contains all overlay data indexed by ID.

    QString db_path = storage + ".sqlite" ;

    bool populate_db = !QFileInfo(db_path).exists() ;
    db_.open((const char *)db_path.toUtf8(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ) ;

    if ( populate_db ) { // create database for the first time
        db_.exec("CREATE TABLE overlays (id INTEGER PRIMARY KEY AUTOINCREMENT, type TEXT NOT NULL, name TEXT NOT NULL, content BLOB NOT NULL);\
                 CREATE TABLE collections (id INTEGER PRIMARY KEY AUTOINCREMENT, folder_id INTEGER NOT NULL, is_visible INTEGER DEFAULT 1, name TEXT, attributes BLOB);\
                CREATE TABLE memberships (overlay_id INTEGER, collection_id INTEGER);\
        CREATE INDEX membership_idx ON memberships (overlay_id);\
        CREATE UNIQUE INDEX membership_unique_idx ON memberships(overlay_id, collection_id); \
        CREATE TABLE folders (id INTEGER PRIMARY KEY AUTOINCREMENT, is_visible INTEGER DEFAULT 1, name TEXT NOT NULL, parent_id INTEGER NOT NULL);\
        INSERT INTO folders (name, parent_id) VALUES ('Library', 0);\
        PRAGMA journal_mode=WAL;PRAGMA synchronous=0;PRAGMA recursive_triggers=ON;") ;
    }

    // Now create the spatial index that uses a paged memory file

    id_type indexIdentifier;
    string index_path_prefix((const char *)storage.toUtf8()) ;

    if ( QFileInfo(storage + ".idx").exists() ) // load existing a spatial index saved on disk
    {
        try {
            storage_ = StorageManager::loadDiskStorageManager(index_path_prefix);
            buffer_ = StorageManager::createNewRandomEvictionsBuffer(*storage_, index_buffer_capacity_, false);

            QSettings sts ;
            indexIdentifier = sts.value("overlays/spatial_index_id", 1).toInt() ;

            index_ = RTree::loadRTree(*storage_, indexIdentifier);

            return true ;
        }
        catch (Tools::IllegalStateException &e )
        {
            // We probably are here due to corrupted index
            cout << "ok here" << endl ;
        }
    }

    // index not exists or corrupted, so create a new one
    storage_ = StorageManager::createNewDiskStorageManager(index_path_prefix, index_page_size_);
    buffer_ = StorageManager::createNewRandomEvictionsBuffer(*storage_, index_buffer_capacity_, false);
    index_ = RTree::createNewRTree(*buffer_, 0.7, 100, 100, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

    QSettings sts ;
    sts.setValue("overlays/spatial_index_id", QVariant::fromValue<quint64>(indexIdentifier)) ;
    return true ;
}


class QueryVisitor : public IVisitor
{
public:
    QueryVisitor(QVector<quint64> &ovr): ovr_(ovr) {}

    void visitNode(const INode& n) {}

    void visitData(const IData& d)
    {
        ovr_.append(d.getIdentifier()) ;

        //cout << d.getIdentifier() << ' ' << (const char *)data << endl ;
    }

    void visitData(std::vector<const IData*>& v) {}

    QVector<quint64> &ovr_ ;
};

MapOverlayPtr MapOverlayManager::findNearest(const QByteArray &searchType, const QPoint &click, MapWidget *view, double thresh)
{

    QVector<quint64> ids ;

    QueryVisitor v(ids) ;

    QPointF coords = view->displayToCoords(click) ;

    double pts[2] ;
    pts[0] = coords.x() ;
    pts[1] = coords.y() ;
    SpatialIndex::Point p(pts, 2) ;

    index_->pointLocationQuery(p, v) ;

    if ( ids.isEmpty() ) return MapOverlayPtr() ;

    QVector<MapOverlayPtr> overlays ;

    getAllOverlays(ids, overlays) ;

    double min_dist = DBL_MAX ;
    MapOverlayPtr best ;

    Q_FOREACH(MapOverlayPtr overlay, overlays)
    {
        if ( searchType != "*" && overlay->type() != searchType )  continue ;

        int seg ;
        double dist = overlay->distanceToPt(click, seg, view) ;

        if ( dist < min_dist )
        {
            min_dist = dist ;
            best = overlay ;
        }
    }

    if ( min_dist > thresh ) best = MapOverlayPtr() ;

    return best ;
}

MapOverlayPtr MapOverlayManager::load(quint64 id)
{
    try {

        SQLite::Query q(db_, "select f.id as id, f.type as type, f.name as name, f.content as content, c.is_visible as is_visible from overlays as f join memberships as m on m.overlay_id = f.id join collections as c on c.id = collection_id where f.id = ?") ;

        q.bind(id) ;

        SQLite::QueryResult res = q.exec() ;

        if ( res )
        {
            string type = res.get<string>(1) ;
            QString name = QString::fromUtf8(res.get<string>(2).c_str()) ;

            MapOverlayPtr obj = MapOverlay::create(type, name) ;

            SQLite::Blob blob = res.get<SQLite::Blob>(3) ;

            QByteArray ba(blob.data(), blob.size()) ;

            obj->deserialize(ba);
            obj->storage_id_ = id ;

            bool is_visible = res.get<bool>(4) ;
            obj->visible_ = is_visible ;

            return obj ;
        }
        else return MapOverlayPtr();

    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return MapOverlayPtr() ;
    }
}

bool MapOverlayManager::update(const MapOverlayPtr &overlay)
{
    // we need to load the old overlay to get its MBR
    MapOverlayPtr old_overlay = load(overlay->id()) ;

    if (!old_overlay ) return false ;

    try {

        SQLite::Statement q(db_, "UPDATE overlays SET name=?, content=? WHERE id=?;") ;

        q.bind(overlay->name().toUtf8().data()) ;

        QByteArray ba = overlay->serialize() ;
        q.bind(SQLite::Blob(ba.data(), ba.size())) ;
        q.bind((long long int)overlay->id()) ;

        q.exec() ;

        // delete old object from spatial index

        double plow[2], phigh[2];

        QRectF mbr = old_overlay->boundingBox().normalized() ;

        plow[0] = std::min(mbr.left(), mbr.right());
        plow[1] = std::min(mbr.top(), mbr.bottom());
        phigh[0] = std::max(mbr.left(), mbr.right());
        phigh[1] = std::max(mbr.top(), mbr.bottom()) ;

        SpatialIndex::Region r_old(plow, phigh, 2);

        index_->deleteData(r_old, overlay->id()) ;

        mbr = overlay->boundingBox().normalized() ;

        plow[0] = std::min(mbr.left(), mbr.right());
        plow[1] = std::min(mbr.top(), mbr.bottom());
        phigh[0] = std::max(mbr.left(), mbr.right());
        phigh[1] = std::max(mbr.top(), mbr.bottom()) ;

        SpatialIndex::Region r_new(plow, phigh, 2);

        std::ostringstream os;
        os << r_new;
        std::string idata = os.str();

        index_->insertData(idata.size() + 1, reinterpret_cast<const byte*>(idata.c_str()), r_new, overlay->id()) ;

        return true ;

    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }

}

bool MapOverlayManager::write(const QVector<MapOverlayPtr> &objects, quint64 collection_id)
{
    SQLite::Transaction trans(db_) ;
    bool res = write(db_, objects, collection_id) ;
    trans.commit() ;

    return res ;
}


bool MapOverlayManager::write(SQLite::Connection &con, const QVector<MapOverlayPtr> &objects, quint64 collection_id)
{
    try {
        // write overlay table
        {

            SQLite::Statement cmd(db_, "INSERT INTO overlays (type, name, content) VALUES (?, ?, ?);") ;

            for(int i=0 ; i<objects.size() ; i++)
            {
                MapOverlayPtr object = objects[i] ;

                QByteArray data = object->serialize() ;
                QByteArray name = object->name().toUtf8() ;

                cmd.bind(object->type().data()) ;
                cmd.bind(name.data()) ;
                cmd.bind(SQLite::Blob(data.data(), data.size())) ;

                cmd.exec() ;

                sqlite3_int64 row_id = con.last_insert_rowid() ;
                object->storage_id_ = row_id ;

                cmd.clear() ;
            }


        }


        {
            // write membership table


            SQLite::Statement cmd(db_, "INSERT INTO memberships (overlay_id, collection_id) VALUES (?, ?)") ;

            for(int i=0 ; i<objects.size() ; i++)
            {
                MapOverlayPtr object = objects[i] ;

                cmd.bind(1, (long long int)object->storage_id_) ;
                cmd.bind(2, (long long int)collection_id) ;

                cmd.exec() ;
                cmd.clear() ;
            }


        }

        for(int i=0 ; i<objects.size() ; i++)
        {
            double plow[2], phigh[2];

            MapOverlayPtr object = objects[i] ;
            QRectF mbr = object->boundingBox().normalized() ;

            plow[0] = std::min(mbr.left(), mbr.right());
            plow[1] = std::min(mbr.top(), mbr.bottom());
            phigh[0] = std::max(mbr.left(), mbr.right());
            phigh[1] = std::max(mbr.top(), mbr.bottom()) ;

            SpatialIndex::Region r(plow, phigh, 2);

            std::ostringstream os;
            os << r;
            std::string idata = os.str();

            index_->insertData(idata.size() + 1, reinterpret_cast<const byte*>(idata.c_str()), r, object->storage_id_) ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e)
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}



void MapOverlayManager::query(QVector<quint64> &ovr, QVector<QRectF> &boxes)
{
    QueryVisitor vis(ovr) ;

    Q_FOREACH(QRectF mbr, boxes)
    {
        double plow[2], phigh[2];

        plow[0] = std::min(mbr.left(), mbr.right());
        plow[1] = std::min(mbr.top(), mbr.bottom());
        phigh[0] = std::max(mbr.left(), mbr.right());
        phigh[1] = std::max(mbr.top(), mbr.bottom()) ;

        SpatialIndex::Region r(plow, phigh, 2);

        index_->intersectsWithQuery(r, vis);

    }
}

QString MapOverlayManager::uniqueFolderName(const QString &name, quint64 &parent_id) {
    QString name_unique ;
    SQLite::Query q(db_, "SELECT id FROM folders WHERE name = ? AND parent_id = ? LIMIT 1;") ;

    int counter = 0 ;

    try {
        name_unique = name ;

        while (1)
        {
            q.bind(1, name_unique.toUtf8().data()) ;
            q.bind(2, parent_id) ;
            SQLite::QueryResult res = q.exec() ;

            if ( res ) name_unique = name + QString(" (%1)").arg(++counter) ;
            else break ;

            q.clear() ;
        }

        return name_unique ;
    }
    catch ( SQLite::Exception &e )
    {
        cout << e.what() << endl;
        return QString() ;
    }
}

bool MapOverlayManager::addNewFolder(const QString &name_unique, quint64 parent_id, quint64 &item_id)
{
    try {

        SQLite::Statement cmd(db_, "INSERT INTO folders (name, parent_id) VALUES (?, ?)") ;
        cmd.bind(name_unique.toUtf8().data()) ;

        long long id = parent_id ;
        cmd.bind(id) ;
        cmd.exec() ;

        item_id = db_.last_insert_rowid() ;

        return true ;

    }
    catch ( SQLite::Exception &e )
    {
        cout << e.what() << endl;
        return false ;
    }


}

QString MapOverlayManager::uniqueCollectionName(const QString &name, quint64 &folder_id) {

    QString name_unique ;
    SQLite::Query q(db_, "SELECT id FROM collections WHERE name = ? AND folder_id = ? LIMIT 1;") ;

    int counter = 0 ;

    try {
        name_unique = name ;

        while (1)
        {
            q.bind(1, name_unique.toUtf8().data()) ;
            q.bind(2, folder_id) ;
            SQLite::QueryResult res = q.exec() ;

            if ( res ) name_unique = name + QString(" (%1)").arg(++counter) ;
            else break ;

            q.clear() ;
        }

        return name_unique ;
    }
    catch ( SQLite::Exception &e )
    {
        cout << e.what() << endl;
        return QString() ;
    }
}


bool MapOverlayManager::addNewCollection(const QString &name_unique, quint64 folder_id, const QMap<QString, QVariant> &attributes, quint64 &item_id)
{
    try {

        string sql = "INSERT INTO collections (folder_id, name, attributes) VALUES (?, ?, ?)" ;

        SQLite::Statement cmd(db_, sql) ;

        cmd.bind((long long int)folder_id) ;
        cmd.bind(name_unique.toUtf8().data()) ;

        QByteArray ba ;
        QDataStream ds(&ba, QIODevice::WriteOnly) ;

        ds << attributes ;

        cmd.bind(SQLite::Blob(ba.data(), ba.size())) ;
        cmd.exec() ;

        item_id = (quint64)db_.last_insert_rowid() ;

        return true ;

    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }


}

bool MapOverlayManager::addCollectionTree(CollectionTreeNode *node, quint64 parent_id) {

    QString unique_name = uniqueFolderName(node->name_, parent_id);
    quint64 item_id ;

    addNewFolder(unique_name, parent_id,  item_id) ;

    node->name_ = unique_name ;
    node->folder_id_ = item_id ;

    if ( !node->overlay_list_.isEmpty() )
    {
        CollectionData *col = new CollectionData ;

        node->collection_ = col ;

        QString unique_col_name = uniqueCollectionName(node->name_, node->folder_id_) ;
        quint64 collection_id ;
        QMap<QString, QVariant> attr ;

        addNewCollection(unique_col_name, node->folder_id_, attr, collection_id) ;

        col->name_ = unique_col_name ;
        col->id_ = collection_id ;
        col->folder_ = item_id ;

        //        write(node->overlay_list_, col->id_ )  ;
    }

    Q_FOREACH(CollectionTreeNode *child, node->children_)
        addCollectionTree(child, item_id) ;
}

bool MapOverlayManager::addCollectionTreeOverlays(SQLite::Connection &con, CollectionTreeNode *node) {

    if ( !node->overlay_list_.empty() ) write(con, node->overlay_list_, node->collection_->id_ )  ;

    Q_FOREACH(CollectionTreeNode *child, node->children_)
        addCollectionTreeOverlays(con, child) ;
}

bool MapOverlayManager::addCollectionTreeOverlays(CollectionTreeNode *node) {

    SQLite::Transaction trans(db_) ;
    bool res = addCollectionTreeOverlays(db_, node) ;
    trans.commit() ;

    return res ;
}


bool MapOverlayManager::addOverlayInCollection(quint64 collection_id, const QVector<quint64> &overlays)
{
    try {

        SQLite::Transaction trans(db_) ;

        SQLite::Statement cmd(db_, "REPLACE INTO memberships (overlay_id, collection_id) VALUES (?, ?);") ;

        Q_FOREACH(quint64 id, overlays)
        {
            cmd.bind(1, (long long)id) ;
            cmd.bind(2, (long long)collection_id) ;

            cmd.exec() ;
            cmd.clear() ;
        }

        trans.commit() ;

        return true ;

    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}

void MapOverlayManager::deleteOverlaysFromSpatialIndex(const QVector<MapOverlayPtr> &objs) {

    Q_FOREACH(MapOverlayPtr obj, objs)
    {
        double plow[2], phigh[2];

        QRectF mbr = obj->boundingBox().normalized() ;

        plow[0] = std::min(mbr.left(), mbr.right());
        plow[1] = std::min(mbr.top(), mbr.bottom());
        phigh[0] = std::max(mbr.left(), mbr.right());
        phigh[1] = std::max(mbr.top(), mbr.bottom()) ;

        SpatialIndex::Region r(plow, phigh, 2);

        index_->deleteData(r, obj->id()) ;
    }
}

bool MapOverlayManager::deleteOverlaysFromCollection(quint64 collection_id, const QVector<quint64> &overlays)
{
    // delete from spatial index. We have to obtain the bounding box from database

    QVector<MapOverlayPtr> objs ;
    getAllOverlays(overlays, objs) ;
    deleteOverlaysFromSpatialIndex(objs);

    try {

        SQLite::Transaction trans(db_) ;

        SQLite::Statement cmd1(db_, "DELETE FROM memberships WHERE overlay_id = ? AND collection_id = ?;") ;

        Q_FOREACH(quint64 id, overlays)
        {
            cmd1.bind(1, id) ;
            cmd1.bind(2, collection_id) ;

            cmd1.exec() ;
            cmd1.clear() ;
        }

        SQLite::Statement cmd2(db_, "DELETE overlays WHERE id = ?;") ;

        Q_FOREACH(quint64 id, overlays)
        {
            cmd2.bind(1, id) ;

            cmd2.exec() ;
            cmd2.clear() ;
        }

        trans.commit() ;

        return true ;

    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}


MapOverlayManager::MapOverlayManager(): index_(0) {
}

MapOverlayManager::~MapOverlayManager() {
    cleanup() ;
}


bool MapOverlayManager::getSubFolders(QVector<quint64> &ids, QVector<QString> &names, QVector<bool> &states, quint64 parent_id)
{
    try {
        SQLite::Query q(db_, "SELECT id, name, is_visible FROM folders where parent_id=?;") ;
        quint64 id = parent_id ;
        q.bind(id) ;
        SQLite::QueryResult res = q.exec() ;

        while ( res )
        {
            quint64 id = res.get<quint64>(0) ;
            string name = res.get<string>(1) ;
            bool state = res.get<bool>(2) ;

            ids.push_back(id) ;
            names.push_back(QString::fromUtf8(name.c_str())) ;
            states.push_back(state) ;

            res.next() ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}


bool MapOverlayManager::getFolderCollections(QVector<quint64> &ids, QVector<QString> &names, QVector<bool> &states, quint64 folder_id)
{
    try {
        SQLite::Query q(db_, "SELECT id, name, is_visible FROM collections where folder_id=?;") ;
        long long int id = folder_id ;
        q.bind(id) ;
        SQLite::QueryResult res = q.exec() ;

        while ( res )
        {
            quint64 id = res.get<quint64>(0) ;
            string name = res.get<string>(1) ;
            bool state = res.get<bool>(2) ;

            ids.push_back(id) ;
            names.push_back(QString::fromUtf8(name.c_str())) ;
            states.push_back(state) ;

            res.next() ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}

int MapOverlayManager::getNumCollections(quint64 folder_id)
{
    try {
        SQLite::Query q(db_, "SELECT count(*) FROM collections where folder_id=?;") ;
        long long int id = folder_id ;
        q.bind(id) ;
        SQLite::QueryResult res = q.exec() ;

        if ( res )
            return res.get<int>(0) ;
        else
            return 0 ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return 0 ;
    }

}

bool MapOverlayManager::getAllOverlays(const QVector<quint64> overlay_ids, QVector<MapOverlayPtr> &overlays)
{
    QByteArray idlist ;

    Q_FOREACH(quint64 id, overlay_ids)
    {
        if ( !idlist.isEmpty() ) idlist += ',' ;
        idlist += QString("%1").arg(id).toLatin1() ;
    }

    try {
        QByteArray sql = "SELECT * FROM overlays WHERE id IN (" ;
        sql += idlist + ')' ;

        SQLite::Query q(db_, sql.data()) ;

        for( const SQLite::Row &r: q.exec() )
        {
            quint64 id =  r[0].as<quint64>() ;
            string type = r[1].as<string>() ;
            string name = r[2].as<string>() ;

            MapOverlayPtr overlay = MapOverlay::create(type, QString::fromUtf8(name.c_str())) ;
            overlay->storage_id_ = id ;

            SQLite::Blob blob = r[3].as<SQLite::Blob>() ;

            QByteArray ba(blob.data(), blob.size()) ;

            overlay->deserialize(ba);

            overlays.append(overlay) ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }

}

bool MapOverlayManager::getAllOverlaysInFolder(quint64 folder_id, QVector<MapOverlayPtr> &overlays) {

    QVector<quint64> cols, folders ;
    QVector<QString> c_names, f_names ;

    QVector<bool> c_states, f_states ;

    getFolderCollections(cols, c_names, c_states, folder_id);
    Q_FOREACH(quint64 col_id, cols) {
        getAllOverlaysInCollection(col_id, overlays) ;
    }

    getSubFolders(folders, f_names, f_states, folder_id) ;

    Q_FOREACH(quint64 fid, folders) {
        getAllOverlaysInFolder(fid, overlays) ;
    }
}

bool MapOverlayManager::getAllOverlaysInCollection(quint64 collection_id, QVector<MapOverlayPtr> &overlays)
{
    try {
        SQLite::Query q(db_, "SELECT f.id, f.type, f.name, f.content FROM overlays AS f JOIN memberships AS m ON m.overlay_id = f.id WHERE m.collection_id = ?;") ;
        long long int id = collection_id ;
        q.bind(id) ;

        for( const SQLite::Row &r: q.exec() )
        {
            quint64 id = r[0].as<quint64>() ;
            string type = r[1].as<string>() ;
            string name = r[2].as<string>() ;

            MapOverlayPtr overlay = MapOverlay::create(type, QString::fromUtf8(name.c_str())) ;
            overlay->storage_id_ = id ;

            SQLite::Blob blob = r[3].as<SQLite::Blob>() ;

            QByteArray ba(blob.data(), blob.size()) ;

            overlay->deserialize(ba);

            overlays.append(overlay) ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }

}

bool MapOverlayManager::deleteCollections(SQLite::Connection &con, quint64 folder_id)
{
    try {
        SQLite::Query q(con, "SELECT id from collections WHERE folder_id = ?") ;
        q.bind(folder_id) ;
        SQLite::QueryResult res = q.exec() ;

        while ( res ) {
            deleteCollection(con, res.get<int>(0)) ;
            res.next() ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}


bool MapOverlayManager::deleteFolders(SQLite::Connection &con, quint64 parent_folder_id)
{
    try {
        SQLite::Statement q(con, "DELETE FROM folders where parent_id=?;") ;
        q.bind(parent_folder_id) ;
        q.exec() ;
        return true ;

    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}


bool MapOverlayManager::deleteFolder(SQLite::Connection &con, quint64 folder_id)
{
    try {

        // remove the folder itself

        SQLite::Statement cmd(con, "DELETE FROM folders where id=?;") ;

        cmd.bind(folder_id) ;
        cmd.exec() ;

        // delete collections in this folder

        deleteCollections(con, folder_id) ;

        // delete all child folders

        SQLite::Query q(con, "SELECT id from folders WHERE parent_id = ?") ;
        q.bind(folder_id) ;
        SQLite::QueryResult res = q.exec() ;

        while ( res ) {
            deleteFolder(con, res.get<int>(0)) ;
            res.next() ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}


bool MapOverlayManager::deleteFolder(quint64 folder_id) {
    SQLite::Transaction trans(db_) ;
    bool res = deleteFolder(db_, folder_id) ;
    trans.commit() ;

    return res ;
}

bool MapOverlayManager::deleteCollection(quint64 id) {
    deleteCollection(db_, id) ;
}

bool MapOverlayManager::deleteCollection(SQLite::Connection &con, quint64 collection_id)
{
    try {

        long long int id = collection_id ;

        SQLite::Statement cmd1(con, "DELETE FROM collections where id=?;") ;

        cmd1.bind(id) ;
        cmd1.exec() ;

        SQLite::Statement cmd2(con, "DELETE FROM overlays WHERE id IN ( SELECT overlay_id FROM memberships WHERE collection_id=?);") ;

        cmd2.bind(id) ;
        cmd2.exec() ;

        SQLite::Statement cmd3(con, "DELETE FROM memberships where collection_id=?;") ;

        cmd3.bind(id) ;
        cmd3.exec() ;

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }

}


bool MapOverlayManager::getOverlayCollectionAndFolder(quint64 overlay_id, quint64 &collection_id, quint64 &folder_id)
{
    try {

        {
            SQLite::Query cmd(db_, "SELECT collection_id from memberships WHERE overlay_id=? LIMIT 1;") ;

            cmd.bind(overlay_id) ;
            SQLite::QueryResult res = cmd.exec() ;

            if ( !res ) return false ;

            collection_id = res.get<quint64>(0) ;
        }

        {

            SQLite::Query cmd(db_, "SELECT folder_id from collections WHERE id=? LIMIT 1;") ;

            cmd.bind(collection_id) ;
            SQLite::QueryResult res = cmd.exec() ;

            if ( !res ) return false ;

            folder_id = res.get<quint64>(0) ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }

}


bool MapOverlayManager::renameFolder(quint64 folder_id, const QString &newName)
{
    try {

        long long int id = folder_id ;

        SQLite::Statement cmd(db_, "UPDATE folders SET name=? WHERE id=?;") ;

        cmd.bind(newName.toUtf8().data()) ;
        cmd.bind(id) ;
        cmd.exec() ;

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}


bool MapOverlayManager::renameCollection(quint64 collection_id, const QString &newName)
{
    try {

        long long int id = collection_id ;

        SQLite::Statement cmd(db_, "UPDATE collections SET name=? WHERE id=?;") ;

        cmd.bind(newName.toUtf8().data()) ;
        cmd.bind(id) ;
        cmd.exec() ;

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}

bool MapOverlayManager::moveFolder(quint64 folder_id, quint64 parent_folder_id)
{
    try {

        long long int id = folder_id ;
        long long int parent_id = parent_folder_id ;

        SQLite::Statement cmd(db_, "UPDATE folders SET parent_id=? WHERE id=?;") ;

        cmd.bind(parent_id) ;
        cmd.bind(id) ;
        cmd.exec() ;

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }

}

bool MapOverlayManager::moveCollection(quint64 collection_id, quint64 parent_folder_id)
{
    try {

        long long int id = collection_id ;
        long long int parent_id = parent_folder_id ;

        SQLite::Statement cmd(db_, "UPDATE collections SET folder_id=? WHERE id=?;") ;

        cmd.bind(parent_id) ;
        cmd.bind(id) ;
        cmd.exec() ;

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }

}

bool MapOverlayManager::setCollectionVisibility(quint64 id, bool state)
{
    try {

        {
            SQLite::Statement cmd(db_, "UPDATE collections SET is_visible=? WHERE id=?;") ;

            cmd.bind(state) ;
            cmd.bind(id) ;
            cmd.exec() ;
        }

        if ( state ) // is checked state we have to check the parent folder too
        {
            db_.exec("CREATE TRIGGER parent_folder_visibility AFTER UPDATE OF is_visible ON folders WHEN NEW.is_visible=1\
                     BEGIN\
                     UPDATE folders SET is_visible=1 WHERE folders.id = NEW.parent_id;\
                    END;") ;


            SQLite::Statement cmd(db_, "UPDATE folders SET is_visible=1 where id IN ( SELECT folder_id FROM collections WHERE id = ? LIMIT 1);") ;
            cmd.bind(id) ;
            cmd.exec() ;

            db_.exec("DROP TRIGGER parent_folder_visibility") ;

        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}

bool MapOverlayManager::setFolderVisibility(quint64 id, bool state, bool update_children)
{
    try {
        // update this folder ( establish a triger that will update recursively child folders and collections)

        {
            db_.exec("CREATE TRIGGER folder_visibility BEFORE UPDATE OF is_visible ON folders\
                     BEGIN\
                        UPDATE folders SET is_visible = NEW.is_visible WHERE folders.parent_id = NEW.id;\
                        UPDATE collections SET is_visible = NEW.is_visible WHERE collections.folder_id = NEW.id;\
                     END;") ;
            SQLite::Statement cmd(db_, "UPDATE folders SET is_visible=? WHERE id=?") ;

            cmd.bind(state) ;
            cmd.bind(id) ;
            cmd.exec() ;

            db_.exec("DROP TRIGGER folder_visibility;") ;
        }

        if ( state ) // is checked state we have to check the parent folder too
        {
            db_.exec("CREATE TRIGGER parent_folder_visibility AFTER UPDATE OF is_visible ON folders WHEN NEW.is_visible=1\
                     BEGIN\
                     UPDATE folders SET is_visible=1 WHERE folders.id = NEW.parent_id;\
                    END;") ;


            SQLite::Statement cmd(db_, "UPDATE folders SET is_visible=1 where id=?;") ;
            cmd.bind(id) ;
            cmd.exec() ;

            db_.exec("DROP TRIGGER parent_folder_visibility") ;
        }


        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return false ;
    }
}

void MapOverlayManager::cleanup()
{
    if ( index_ )
    {
        delete index_;
        delete buffer_;
        delete storage_;
        index_ = nullptr ;
    }
}

QString MapOverlayManager::uniqueOverlayName(const QString &pattern, quint64 collection_id, int &counter)
{
    try {

        SQLite::Query q(db_, "SELECT f.id FROM overlays AS f JOIN memberships AS m ON m.overlay_id = f.id WHERE f.name = ? AND m.collection_id = ? LIMIT 1;") ;

        QString name_unique = pattern.arg((int)counter, 3, 10, QChar('0')) ;

        while (1)
        {
            ++counter ;

            q.bind(1, name_unique.toUtf8().data()) ;
            q.bind(2, (long long)collection_id) ;

            SQLite::QueryResult res = q.exec() ;

            if ( res ) name_unique = pattern.arg((int)counter, 3, 10, QChar('0')) ;
            else break ;

            q.clear() ;
        }

        return name_unique ;
    }
    catch ( SQLite::Exception &e )
    {
        qDebug() <<  e.what() ;
        return QString() ;
    }

}

QRectF MapOverlayManager::getOverlayBBox(const QVector<quint64> &overlay_ids)
{
    QVector<MapOverlayPtr> overlays ;

    getAllOverlays(overlay_ids, overlays) ;

    QRectF box ;

    Q_FOREACH(MapOverlayPtr overlay, overlays)  {
        box = box.united(overlay->boundingBox()) ;
    }

    return box ;
}



QRectF MapOverlayManager::getCollectionBBox(quint64 collection_id)
{
    QVector<MapOverlayPtr> overlays ;

    getAllOverlaysInCollection(collection_id, overlays) ;

    QRectF box ;

    Q_FOREACH(MapOverlayPtr overlay, overlays) {
        box = box.united(overlay->boundingBox()) ;
    }

    return box ;
}


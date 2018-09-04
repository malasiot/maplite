#include "osm_storage_db.hpp"

using namespace std ;

namespace OSM {

void DBStorage::writeNode(const Node &n) {
    try {
        string sql = "INSERT INTO osm_nodes (osm_id, tags, lat, lon) VALUES ( ?, ?, ?, ?)" ;
        string tagData = serializeTags(n.tags_) ;
        SQLite::Blob tagsBlob(tagData.data(), tagData.size()) ;
        SQLite::Statement stmt(db_, sql, n.id_, tagsBlob, n.lat_, n.lon_) ;
        stmt.exec() ;
    } catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }

}

void DBStorage::writeWay(const Way &way) {
    try {

        string sql = "INSERT INTO osm_ways (osm_id, tags) VALUES (?, ?)" ;
        string tagData = serializeTags(way.tags_) ;
        SQLite::Blob tagsBlob(tagData.data(), tagData.size()) ;
        SQLite::Statement(db_, sql, way.id_, tagsBlob).exec() ;

        sql = "INSERT INTO osm_ways_nodes (way, node) VALUES (?, ?)" ;
        SQLite::Statement stmt(db_, sql) ;

        for( const osm_id_t &nd: way.nodes_ )  {
            stmt.clear() ;
            stmt.bindm(way.id_, nd) ;
            stmt.exec() ;
        }
    } catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }

}

void DBStorage::writeRelation(const Relation &r) {

}

bool DBStorage::readWay(osm_id_t id, Way &way)
{
    string sql = "SELECT osm_id, tags FROM osm_ways WHERE id = ?" ;

     try {
        SQLite::Query q(db_, sql, id) ;
        SQLite::QueryResult res = q.exec() ;

        if ( res ) {
            osm_id_t id = res.get<osm_id_t>(0) ;
            SQLite::Blob b = res.get<SQLite::Blob>(1) ;

            way.id_ = id ;
            way.tags_ = decodeTags(b.data(), b.size()) ;
            return true ;
        }
    } catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return false ;
}

void DBStorage::forAllNodes(std::function<void (const Node &)> cf) {
    string sql = "SELECT osm_id, tags, lat, lon FROM osm_nodes" ;

     try {
        SQLite::Query q(db_, sql) ;

        for( const SQLite::Row &r: q.exec() ) {

            Node n ;
            n.id_ = r[0].as<osm_id_t>() ;
            SQLite::Blob b = r[1].as<SQLite::Blob>() ;
            n.tags_ = decodeTags(b.data(), b.size()) ;
            n.lat_ = r[2].as<float>() ;
            n.lon_ = r[3].as<float>() ;

            cf(n) ;
        }
    } catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }
}

void OSM::DBStorage::initTables() {
    string sql = "CREATE TABLE IF NOT EXISTS osm_nodes (osm_id INTEGER, tags BLOB, lat FLOAT, lon FLOAT)" ;
    SQLite::Statement(db_, sql).exec() ;

    sql = "CREATE TABLE IF NOT EXISTS osm_ways (osm_id INTEGER PRIMARY KEY, tags BLOB)" ;
    SQLite::Statement(db_, sql).exec() ;

    sql = "CREATE TABLE IF NOT EXISTS osm_ways_nodes (way INTEGER, node INTEGER, UNIQUE(way, node))" ;
    SQLite::Statement(db_, sql).exec() ;

    sql = "CREATE TABLE IF NOT EXISTS osm_relations (osm_id INTEGER, tags BLOB)" ;
    SQLite::Statement(db_, sql).exec() ;

    sql = "CREATE TABLE IF NOT EXISTS osm_relations_nodes (rel, node, UNIQUE(rel, node))" ;
    SQLite::Statement(db_, sql).exec() ;

    sql = "CREATE TABLE IF NOT EXISTS osm_relations_ways (rel, way, UNIQUE(rel, way))" ;
    SQLite::Statement(db_, sql).exec() ;

    sql = "CREATE TABLE IF NOT EXISTS osm_relations_relations (rel, child, UNIQUE(rel, child))" ;
    SQLite::Statement(db_, sql).exec() ;
}

string DBStorage::serializeTags(const Dictionary &tags) {
    string res ;
    for( const auto &kv: tags ) {
        res += kv.first ;
        res += '\0' ;
        if ( !kv.second.empty() ) res += kv.second ;
        res += '\0' ;
    }

    res += '\0' ;

    return res ;
}

Dictionary DBStorage::decodeTags(const char *data, size_t sz) {
    Dictionary tags ;

    const char *c = data ;
    while ( *c ) {
        string key, val ;
        while ( *c ) key += *c++ ;
        ++c ;
        while ( *c ) val += *c++ ;
        ++c ;
        tags.add(key, val) ;
    }
    return tags ;
}
}

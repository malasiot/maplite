#include "mapsforge_poi_writer.hpp"

#include <sstream>
#include <boost/filesystem.hpp>

using namespace std ;

void POIWriter::create(const std::string &name)
{
    if ( boost::filesystem::exists(name) )
        boost::filesystem::remove(name);

    db_.open(name, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE) ;

    db_.exec("DROP TABLE IF EXISTS poi_data;") ;
    db_.exec("DROP TABLE IF EXISTS metadata;");
    db_.exec("DROP TABLE IF EXISTS poi_key;");

    db_.exec("PRAGMA synchronous=OFF") ;
    db_.exec("PRAGMA journal_mode=WAL") ;
    db_.exec("SELECT InitSpatialMetadata(1);") ;
    db_.exec("PRAGMA encoding=\"UTF-8\"") ;

    db_.exec("CREATE TABLE metadata (key TEXT, value TEXT)") ;
    db_.exec("CREATE TABLE poi_data (id INTEGER, name TEXT, name_nrm TEXT, area TEXT, PRIMARY KEY (id))") ;
    db_.exec("CREATE VIRTUAL TABLE poi_key USING fts5(name_nrm)") ;
    db_.exec("SELECT AddGeometryColumn( 'poi_data', 'geom', 4326, 'POINT', 2);") ;
    db_.exec("SELECT CreateSpatialIndex('poi_data', 'geom');") ;

}

void POIWriter::write(OSMProcessor &proc, const POICategoryContainer &categories, POIWriteOptions &options)
{
    writeMetadata() ;
 //   writeCategories(categories) ;
    writePOIData(options) ;
}

void POIWriter::setBoundingBox(const BBox &box) {
    info_.min_lat_ = box.miny_ ;
    info_.min_lon_ = box.minx_ ;
    info_.max_lat_ = box.maxy_ ;
    info_.max_lon_ = box.maxx_ ;

    info_.flags_ |= 0x02 ;
}

void POIWriter::setPreferredLanguages(const std::string &langs) {
    info_.languages_ = langs ;
    info_.flags_ |= 0x10 ;
}

void POIWriter::setComment(const std::string &comment) {
    info_.comment_ = comment ;
    info_.flags_ |= 0x08 ;
}

void POIWriter::setCreator(const std::string &creator) {
    info_.created_by_ = creator ;
    info_.flags_ |= 0x04 ;
}

void POIWriter::setDebug(bool debug) {
    info_.debug_ = debug ;
    if ( debug ) info_.flags_ |= 0x80 ;
}

void POIWriter::setDate(time_t t) {
    info_.date_ = t ;
    info_.flags_ |= 0x20 ;
}

template<class T>
static void addMetaData(SQLite::Statement &stmt, bool has_value, const string &key, const T &value) {
    stmt.clear() ;
    if ( has_value )
        stmt.bindm(key, value) ;
    else
        stmt.bindm(key, SQLite::Nil) ;
    stmt.exec() ;
}

void POIWriter::writeMetadata()
{
    SQLite::Statement stmt(db_, "INSERT INTO metadata (key, value) VALUES (?, ?);") ;

    addMetaData(stmt, info_.flags_ & 0x80, "debug", info_.debug_) ;
    addMetaData(stmt, info_.flags_ & 0x04, "creator", info_.created_by_) ;
    addMetaData(stmt, info_.flags_ & 0x08, "comment", info_.comment_) ;
    addMetaData(stmt, info_.flags_ & 0x10, "languages", info_.languages_) ;
    addMetaData(stmt, info_.flags_ & 0x20, "date", info_.date_) ;

    string bbox_str ;
    if ( info_.flags_ & 0x02 ) {
        stringstream bstrm ;
        bstrm << info_.min_lat_ << ',' << info_.min_lon_ << ',' << info_.max_lat_ << ',' << info_.max_lon_ ;
        bbox_str = bstrm.str() ;
    }

    addMetaData(stmt, info_.flags_ & 0x02, "bounds", bbox_str) ;
    addMetaData(stmt, true, "version", info_.version_) ;
}

void POIWriter::writePOIData(POIWriteOptions &options)
{
    SQLite::Query q(db_, "SELECT * FROM ") ;
}



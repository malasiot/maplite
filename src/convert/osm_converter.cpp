#include "osm_converter.hpp"
#include "geom_utils.hpp"
#include "osm_storage_memory.hpp"

#include <spatialite.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <shapefil.h>
#include <iomanip>
#include <iostream>

using namespace std ;


class SpatialLiteSingleton
{
public:

    static SpatialLiteSingleton instance_;

    static SpatialLiteSingleton& instance() {
        return instance_;
    }

private:

    SpatialLiteSingleton () {
        spatialite_init(false);
    }

    ~SpatialLiteSingleton () {
        spatialite_cleanup();
    }

    SpatialLiteSingleton( SpatialLiteSingleton const & );

    void operator = ( SpatialLiteSingleton const & );
};

SpatialLiteSingleton SpatialLiteSingleton::instance_ ;

bool OSMConverter::createGeometriesTable(const std::string &desc)
{
    try {
        string sql ;

        sql = "CREATE TABLE geom_" + desc;
        sql += " (osm_id INTEGER PRIMARY KEY, osm_type INTEGER" ;

        for ( const auto &tag: tags_ ) {
            sql += ", \"" + tag + "\" TEXT DEFAULT NULL";
        }

        sql += ")" ;

        SQLite::Statement(db_, sql).exec() ;

        // Add geometry column

        string geom_type ;

        if ( desc == "lines" )
            geom_type = "MULTILINESTRING" ;
        else if ( desc == "polygons")
            geom_type = "MULTIPOLYGON" ;
        else if ( desc == "pois")
            geom_type = "POINT" ;
        else if ( desc == "relations" )
            geom_type = "MULTILINESTRING" ;

        sql = "SELECT AddGeometryColumn( 'geom_" + desc + "', 'geom', 4326, '" + geom_type + "', 2);" ;
        db_.exec(sql) ;

        // create spatial index

        db_.exec("SELECT CreateSpatialIndex('geom_" + desc + "', 'geom');") ;

        return true ;
    }
    catch ( SQLite::Exception &e)
    {
        cerr << e.what() << endl ;
        return false ;
    }
}

static string insert_feature_sql(const string &desc, std::set<string> tags, const string &geom_cmd = "?" )
{
    string sql ;

    sql = "INSERT INTO geom_" + desc ;
    sql += " (geom, osm_id, osm_type" ;

    for ( const auto &tag: tags ) {
        sql += ", \"" + tag + "\"";
    }

    sql += ") VALUES (" + geom_cmd + ",?, ?" ;

    for ( const auto &tag: tags ) {
        sql += ", ?";
    }

    sql += ")" ;

    return sql ;
}

static void get_geometry_extent(SQLite::Connection &con, const string &desc, double &min_lat, double &min_lon, double &max_lat, double &max_lon) {
    string sql = "SELECT Extent(geom) from geom_" + desc ;

    SQLite::Query q(con, sql) ;

    SQLite::QueryResult res = q.exec() ;

    if ( res ) {


        SQLite::Blob blob = res.get<SQLite::Blob>(0) ;

        if ( blob.data() == nullptr ) return ;

        gaiaGeomCollPtr geom = gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)blob.data(), blob.size());

        gaiaPolygonPtr poly = geom->FirstPolygon ;
        gaiaRingPtr ring = poly->Exterior ;

        double *c = ring->Coords ;

        for( uint i=0 ; i<ring->Points ; i++ ) {
            double lon = *c++, lat = *c++ ;

            min_lat = std::min<float>(min_lat, lat) ;
            min_lon = std::min<float>(min_lon, lon) ;
            max_lat = std::max<float>(max_lat, lat) ;
            max_lon = std::max<float>(max_lon, lon) ;
        }
    }
}


bool OSMConverter::create(const std::string &name, const std::string &tags) {

    if ( boost::filesystem::exists(name) )
        boost::filesystem::remove(name);

    vector<string> vtags;
    boost::split(vtags, tags, boost::is_any_of(";"));

    std::copy(vtags.begin(), vtags.end(), std::inserter(tags_, tags_.begin())) ;

    db_.open(name, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE) ;

    try {
        db_.exec("PRAGMA synchronous=OFF") ;
        db_.exec("PRAGMA journal_mode=WAL") ;
        db_.exec("SELECT InitSpatialMetadata(1);") ;
        db_.exec("PRAGMA encoding=\"UTF-8\"") ;

        return ( createGeometriesTable("lines") &&
                 createGeometriesTable("pois") &&
                 createGeometriesTable("polygons")

                 ) ;
    }
    catch ( SQLite::Exception &e )
    {
        cerr << e.what()<< endl ;
        return false ;
    }
}

bool OSMConverter::addLineGeometry(SQLite::Statement &cmd, OSM::Storage &reader, const OSM::Way &way)
{
    try {
        if ( way.nodes_.size() < 2 ) return false ;

        gaiaGeomCollAutoPtr geo_line = makeLineString(way, reader) ;
        if ( geo_line == nullptr ) return false ;


        WKBBuffer buffer(geo_line) ;

        cmd.bind(buffer.blob()) ;
        cmd.bind(way.id_) ;
        cmd.bind(osm_way_t) ;

        for ( const string &tag: tags_ ) {
            if ( way.tags_.contains(tag) ) {
                cmd.bind(way.tags_.get(tag)) ;
            } else
                cmd.bind(SQLite::Nil) ;
        }

        cmd.exec() ;
        cmd.clear() ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}

bool OSMConverter::addMultiLineGeometry(SQLite::Statement &cmd, OSM::Storage &reader, const std::vector<OSM::Way> &ways, const Dictionary &tags, osm_id_t id, osm_feature_t ftype)
{

    try {
        gaiaGeomCollAutoPtr geo_mline = makeMultiLineString(ways, reader) ;
        if ( geo_mline == nullptr ) return false ;

        WKBBuffer buffer(geo_mline) ;

        cmd.bind(buffer.blob()) ;
        cmd.bind(id) ;
        cmd.bind(ftype) ;

        for ( const string &tag: tags_ ) {
            if ( tags.contains(tag) ) {
                cmd.bind(tags.get(tag)) ;
            } else
                cmd.bind(SQLite::Nil) ;
        }


        cmd.exec() ;
        cmd.clear() ;
        return true ;
     }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
}

bool OSMConverter::addPointGeometry(SQLite::Statement &cmd, const OSM::Node &poi)
{
    try {
        gaiaGeomCollAutoPtr geo_pt = makePoint(poi) ;
        WKBBuffer buffer(geo_pt) ;

        cmd.bind(buffer.blob()) ;
        cmd.bind(poi.id_) ;
        cmd.bind(osm_node_t) ;

        for ( const string &tag: tags_ ) {
            if ( poi.tags_.contains(tag) ) {
                cmd.bind(poi.tags_.get(tag)) ;
            } else
                cmd.bind(SQLite::Nil) ;
        }
        cmd.exec() ;
        cmd.clear() ;
        return true ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
}


bool OSMConverter::addPolygonGeometry(SQLite::Statement &cmd, OSM::Storage &reader, const OSM::Polygon &poly, osm_id_t id, osm_feature_t ftype)
{
    try {
        gaiaGeomCollAutoPtr geom ;

        if ( poly.rings_.size() == 1 )  // simple polygon case
            geom = makeSimplePolygon(poly.rings_[0], reader) ;
        else  // multipolygon, use gaiaPolygonize to find outer and inner rings
            geom = makeMultiPolygon(poly, reader) ;

        if ( geom ) {
            WKBBuffer buffer(geom) ;

            cmd.bind(buffer.blob()) ;
            cmd.bind(id) ;
            cmd.bind(ftype) ;

            for ( const string &tag: tags_ ) {
                if ( poly.tags_.contains(tag) ) {
                    cmd.bind(poly.tags_.get(tag)) ;
                } else
                    cmd.bind(SQLite::Nil) ;
            }

            cmd.exec() ;
            cmd.clear() ;
            return true ;
        }
        else {
            cerr << "invalid multi-polygon (" << id << ")" << endl ;
            return false ;
        }
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
}

OSMConverter::OSMConverter(const string &out_file_path, const string &tags)
{
    create(out_file_path, tags) ;

}


bool OSMConverter::processOsmFile(const string &osm_file)
{
    using namespace tag_filter ;

    // read files from memory and write to spatialite database

    uint8_t zmin, zmax ;

    try {

        OSM::InMemoryStorage storage;
        OSM::DocumentReader reader ;

        cout << "Parsing file: " << osm_file << endl ;

        if ( !reader.read(osm_file, storage) ) {
            cerr << "Error reading from " << osm_file << endl ;
            return false ;
        }

        // group all insertions into a transaction

        SQLite::Transaction trans(db_) ;

        SQLite::Statement cmd_pois(db_, insert_feature_sql("pois", tags_)) ;
        SQLite::Statement cmd_lines(db_, insert_feature_sql("lines", tags_, "ST_Multi(?)")) ;
        SQLite::Statement cmd_polygons(db_, insert_feature_sql("polygons", tags_, "ST_Multi(?)")) ;

        // POIs

        storage.forAllNodes([&] ( const OSM::Node &node )
        {
            if ( node.tags_.empty() ) return ;
            addPointGeometry(cmd_pois, node) ;
        }) ;

        // relations of type route, merge ways into chunks


        storage.forAllRelations([&] ( const OSM::Relation &relation )
        {
            string rel_type = relation.tags_.get("type") ;

            if ( rel_type == "route" ) {

                vector<OSM::Way> chunks ;
                if ( !storage.makeWaysFromRelation(relation, chunks) ) return ;

                if ( !chunks.empty() ) {
                    addMultiLineGeometry(cmd_lines, storage, chunks, relation.tags_, relation.id_, osm_relation_t) ;

                }
            }
            else if ( rel_type == "multipolygon"  ) {


                OSM::Polygon polygon ;
                if ( !storage.makePolygonsFromRelation(relation, polygon) ) return ;

                if ( !polygon.rings_.empty() ) {
                    addPolygonGeometry(cmd_polygons, storage, polygon, relation.id_, osm_relation_t) ;
 //                   addTags(ctx.tw_, relation.id_, osm_relation_t) ;
                }
            }
        }) ;

        // ways

        storage.forAllWays([&] ( const OSM::Way &way )
        {
            if ( way.tags_.empty() ) return ;

            // deal with closed ways, potential polygon geometries (areas) are those indicated by area tag or those other than highway, barrier and contour

            if ( ( way.nodes_.front() == way.nodes_.back() ) &&
                 ( way.tags_.get("area") == "yes" ||
                   ( !way.tags_.contains("highway") &&
                     !way.tags_.contains("barrier") &&
                     !way.tags_.contains("contour") ) ) ) {

                OSM::Polygon poly ;

                OSM::Ring ring ;
                ring.nodes_.insert(ring.nodes_.end(), way.nodes_.begin(), way.nodes_.end()) ;
                poly.rings_.push_back(ring) ;

                addPolygonGeometry(cmd_polygons, storage, poly, way.id_, osm_way_t) ;
 //               addTags(ctx.tw_, way.id_, osm_way_t) ;
            }
            else {
                addLineGeometry(cmd_lines, storage, way) ;
 //               addTags(ctx.tw_, way.id_, osm_way_t) ;
            }

        }) ;

        trans.commit() ;

        return true ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
    catch ( LuaException &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
}

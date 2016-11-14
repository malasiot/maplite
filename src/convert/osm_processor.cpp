#include "osm_processor.hpp"

#include <spatialite.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
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

bool OSMProcessor::createGeometriesTable(const std::string &desc)
{
    try {
        string sql ;

        sql = "CREATE TABLE geom_" + desc;
        sql += " (osm_id INTEGER PRIMARY KEY, osm_type INTEGER, zmin INTEGER, zmax INTEGER)" ;

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

// OSM ids are not unique across object type (node, way, relation) so we add an integer osm_type along with it
bool OSMProcessor::createTagsTable()
{
    try {
        string sql ;

        sql = "CREATE TABLE kv ";
        sql += "(key TEXT, val TEXT, osm_id INTEGER, osm_type INTEGER, zoom_min INTEGER, zoom_max INTEGER);CREATE INDEX kv_index ON kv(osm_id)" ;

        db_.exec(sql) ;

        return true ;
    }
    catch ( SQLite::Exception &e)
    {
        cerr << e.what() << endl ;
        return false ;
    }
}


static string insert_feature_sql(const string &desc, const string &geom_cmd = "?" )
{
    string sql ;

    sql = "INSERT INTO geom_" + desc ;
    sql += " (geom, osm_id, osm_type, zmin, zmax) VALUES (" + geom_cmd + ",?, ?, ?, ?)";
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

BBox OSMProcessor::getBoundingBoxFromGeometries() {

    double min_lat, min_lon, max_lat, max_lon ;

    min_lat = min_lon = std::numeric_limits<double>::max() ;
    max_lat = max_lon = -std::numeric_limits<double>::max() ;

    get_geometry_extent(db_, "pois", min_lat, min_lon, max_lat, max_lon) ;
    get_geometry_extent(db_, "lines", min_lat, min_lon, max_lat, max_lon) ;
    get_geometry_extent(db_, "polygons", min_lat, min_lon, max_lat, max_lon) ;

    return BBox(min_lon, min_lat, max_lon, max_lat) ;
}

bool OSMProcessor::create(const std::string &name) {

    if ( boost::filesystem::exists(name) )
        boost::filesystem::remove(name);

    db_.open(name, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE) ;

    try {
        db_.exec("PRAGMA synchronous=NORMAL") ;
        db_.exec("PRAGMA journal_mode=WAL") ;
        db_.exec("SELECT InitSpatialMetadata(1);") ;
        db_.exec("PRAGMA encoding=\"UTF-8\"") ;

        return ( createTagsTable() &&
                 createGeometriesTable("lines") &&
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

bool OSMProcessor::addLineGeometry(SQLite::Statement &cmd, OSM::DocumentReader &reader, const OSM::Way &way, uint8_t zmin, uint8_t zmax)
{
    try {
        if ( way.nodes_.size() < 2 ) return false ;

        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_line = gaiaAllocGeomColl();
        geo_line->Srid = 4326;
        geo_line->DeclaredType = GAIA_MULTILINESTRING ;

        gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_line, way.nodes_.size());

        int j = 0 ;
        reader.forAllWayCoords(way.id_, [&](osm_id_t id, double lat, double lon) {
            gaiaSetPoint (ls->Coords, j, lon, lat); ++j ;
        }) ;

        if ( gaiaIsValid(geo_line) ) {

            gaiaToSpatiaLiteBlobWkb (geo_line, &blob, &blob_size);

            cmd.bindm(SQLite::Blob((const char *)blob, blob_size), way.id_, 1, zmin, zmax) ;
            cmd.exec() ;
            cmd.clear() ;

            gaiaFree(blob) ;
        }

        gaiaFreeGeomColl (geo_line);
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}

bool OSMProcessor::addMultiLineGeometry(SQLite::Statement &cmd, OSM::DocumentReader &reader, const std::vector<OSM::Way> &ways, int64_t id, int ftype, uint8_t zmin, uint8_t zmax)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_mline = gaiaAllocGeomColl();
        geo_mline->Srid = 4326;
        geo_mline->DeclaredType = GAIA_MULTILINESTRING ;

        for( auto &way: ways ) {

            if ( way.nodes_.size() < 2 ) continue ;

            gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_mline, way.nodes_.size());

            int j=0 ;
            reader.forAllNodeCoordList(way.nodes_, [&](double lat, double lon) {
                gaiaSetPoint (ls->Coords, j, lon, lat); ++j ;
            }) ;

        }

        if ( gaiaIsValid(geo_mline) ) {
            gaiaToSpatiaLiteBlobWkb (geo_mline, &blob, &blob_size);

            cmd.bindm(SQLite::Blob((const char *)blob, blob_size), id, ftype, zmin, zmax) ;
            cmd.exec() ;
            cmd.clear() ;

            gaiaFree(blob) ;
        }

        gaiaFreeGeomColl (geo_mline);
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}

bool OSMProcessor::addPointGeometry(SQLite::Statement &cmd, const OSM::Node &poi, uint8_t zmin, uint8_t zmax)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_pt = gaiaAllocGeomColl();
        geo_pt->DeclaredType = GAIA_POINT ;
        geo_pt->Srid = 4326;

        gaiaAddPointToGeomColl (geo_pt, poi.lon_, poi.lat_);

        gaiaToSpatiaLiteBlobWkb (geo_pt, &blob, &blob_size);

        cmd.bindm(SQLite::Blob((const char *)blob, blob_size), poi.id_, 0, zmin, zmax) ;

        cmd.exec() ;

        gaiaFreeGeomColl (geo_pt);
        gaiaFree(blob) ;

        cmd.clear() ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

}


bool OSMProcessor::addPolygonGeometry(SQLite::Statement &cmd, OSM::DocumentReader &reader, const OSM::Polygon &poly, int64_t id, int ftype,
                                      uint8_t zmin, uint8_t zmax)
{
    try {
        unsigned char *blob;
        int blob_size;
        gaiaGeomCollPtr geom = nullptr ;

        if ( poly.rings_.size() == 1 ) { // simple polygon case
            geom = gaiaAllocGeomColl();
            geom->Srid = 4326;
            geom->DeclaredType = GAIA_MULTIPOLYGON ;

            const OSM::Ring &ring = poly.rings_[0] ;

            gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl(geom, ring.nodes_.size(), 0);

            int j=0 ;
            reader.forAllNodeCoordList(ring.nodes_, [&](double lat, double lon) {
                gaiaSetPoint (g_poly->Exterior->Coords, j, lon, lat); ++j ;
            }) ;

        }
        else { // multipolygon, use gaiPolygonize to find outer and inner rings
            gaiaGeomCollPtr ls_geom = gaiaAllocGeomColl();
            ls_geom->Srid = 4326;

            for( uint i=0 ; i<poly.rings_.size() ; i++ ) {
                const OSM::Ring &ring = poly.rings_[i] ;
                gaiaLinestringPtr ls =  gaiaAddLinestringToGeomColl(ls_geom, ring.nodes_.size()) ;

                int j=0 ;
                reader.forAllNodeCoordList(ring.nodes_, [&](double lat, double lon) {
                    gaiaSetPoint (ls->Coords, j, lon, lat); ++j ;
                }) ;
            }

            // at the moment the functions bellow ignore invalid (self-intersecting) polygons
            // self-intersection can be handled at the level of the multi-polygon parsing function (makePolygonsFromRelation)
            gaiaGeomCollPtr ps = gaiaSanitize(ls_geom) ;

            gaiaFreeGeomColl(ls_geom) ;

            if ( ps ) {
               geom = gaiaPolygonize(ps, 1) ;
               gaiaFreeGeomColl(ps) ;
            }
        }


        if ( geom ) {

        /*
        gaiaOutBuffer wkt ;
        gaiaOutBufferInitialize (&wkt);
        gaiaOutWkt(&wkt, geo_poly) ;
*/
            gaiaToSpatiaLiteBlobWkb (geom, &blob, &blob_size);

            cmd.bindm(SQLite::Blob((const char *)blob, blob_size), id, ftype, zmin, zmax) ;
            cmd.exec() ;
            cmd.clear() ;
            gaiaFree(blob) ;
            gaiaFreeGeomColl (geom);
        }
        else {
            cerr << "invalid multi-polygon (" << id << ")" << endl ;
        }



    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}


bool OSMProcessor::addTags(SQLite::Statement &cmd, const TagWriteList &tags, int64_t id, int ftype)
{
    try {
        for( const TagWriteAction &kv: tags.actions_) {
            cmd.bindm(kv.key_, kv.val_, id,  ftype, kv.zoom_min_, kv.zoom_max_) ;
            cmd.exec() ;
            cmd.clear() ;
        }
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }
}


bool OSMProcessor::processOsmFile(const string &osm_file, TagFilter &cfg)
{
    using namespace tag_filter ;

    // read files from memory and write to spatialite database

    uint8_t zmin, zmax ;

    try {

        SQLite::Statement cmd_tags(db_, "INSERT INTO kv (key, val, osm_id, osm_type, zoom_min, zoom_max) VALUES (?, ?, ?, ?, ?, ?)") ;

        OSM::DocumentReader reader ;

        cout << "Parsing file: " << osm_file << endl ;

        if ( !reader.read(osm_file) ) {
            cerr << "Error reading from " << osm_file << endl ;
            return false ;
        }

        // group all insertions into a transaction

        SQLite::Transaction trans(db_) ;

        // POIs

        reader.forAllNodes([&] ( const OSM::Node &node )
        {
            if ( node.tags_.empty() ) return ;

            TagFilterContext ctx(node, &reader) ;
            if ( !cfg.match(ctx, zmin, zmax) ) return ;

            SQLite::Statement cmd_poi(db_, insert_feature_sql("pois")) ;

            addPointGeometry(cmd_poi, node, zmin, zmax) ;
            addTags(cmd_tags, ctx.tw_, node.id_, 0) ;
        }) ;

        // relations of type route, merge ways into chunks


        reader.forAllRelations([&] ( const OSM::Relation &relation )
        {
            string rel_type = relation.tags_.get("type") ;

            if ( rel_type == "route" ) {

                TagFilterContext ctx(relation.tags_, relation.id_,  TagFilterContext::Relation) ;
                if ( !cfg.match(ctx, zmin, zmax) ) return ;

                vector<OSM::Way> chunks ;
                if ( !reader.makeWaysFromRelation(relation, chunks) ) return ;

                SQLite::Statement cmd_rel(db_, insert_feature_sql("lines", "ST_Multi(?)")) ;

                if ( !chunks.empty() ) {
                    addMultiLineGeometry(cmd_rel, reader, chunks, relation.id_, 2, zmin, zmax) ;
                    addTags(cmd_tags, ctx.tw_, relation.id_, 2) ;
                }
            }
            else if ( rel_type == "multipolygon"  ) {

                TagFilterContext ctx(relation.tags_, relation.id_,  TagFilterContext::Way) ;
                if ( !cfg.match(ctx, zmin, zmax) ) return ;

                OSM::Polygon polygon ;
                if ( !reader.makePolygonsFromRelation(relation, polygon) ) return ;

                SQLite::Statement cmd_rel(db_, insert_feature_sql("polygons", "ST_Multi(?)")) ;

                if ( !polygon.rings_.empty() ) {
                    addPolygonGeometry(cmd_rel, reader, polygon, relation.id_, 2, zmin, zmax) ;
                    addTags(cmd_tags, ctx.tw_, relation.id_, 2) ;
                }

            }
        }) ;

        // ways

        reader.forAllWays([&] ( const OSM::Way &way )
        {
            if ( way.tags_.empty() ) return ;

            //     if ( member_of_multipolygon_or_boundary(way, reader) ) return ;

            // match feature with filter rules

            TagFilterContext ctx(way, &reader) ;
            if ( !cfg.match(ctx, zmin, zmax) )
                return ;

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

                SQLite::Statement cmd_poly(db_, insert_feature_sql("polygons", "ST_Multi(?)")) ;

                addPolygonGeometry(cmd_poly, reader, poly, way.id_, 1, zmin, zmax) ;
                addTags(cmd_tags, ctx.tw_, way.id_, 1) ;
            }
            else {
                SQLite::Statement cmd_line(db_, insert_feature_sql("lines", "ST_Multi(?)")) ;


                addLineGeometry(cmd_line, reader, way, zmin, zmax) ;
                addTags(cmd_tags, ctx.tw_, way.id_, 1) ;
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


struct DBField {
    DBFFieldType type_ ;
    int width_, precision_ ;
    char name_[12] ;
};

static bool write_box_geometry(SQLite::Connection &con, const BBox &box, osm_id_t id ) {

    SQLite::Statement cmd(con, insert_feature_sql("polygons", "ST_Multi(?)")) ;

    unsigned char *blob;
    int blob_size;

    gaiaGeomCollPtr geo_poly = gaiaAllocGeomColl();
    geo_poly->Srid = 4326;
    geo_poly->DeclaredType = GAIA_POLYGON ;

    gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl (geo_poly, 5, 0) ;
    gaiaRingPtr er = g_poly->Exterior ;

    gaiaSetPoint (er->Coords, 0, box.minx_, box.miny_);
    gaiaSetPoint (er->Coords, 1, box.maxx_, box.miny_);
    gaiaSetPoint (er->Coords, 2, box.maxx_, box.maxy_);
    gaiaSetPoint (er->Coords, 3, box.minx_, box.maxy_);
    gaiaSetPoint (er->Coords, 4, box.minx_, box.miny_);

    gaiaToSpatiaLiteBlobWkb (geo_poly, &blob, &blob_size);

    cmd.bindm(SQLite::Blob((const char *)blob, blob_size), id, 0, 255) ;
    cmd.exec() ;

    gaiaFreeGeomColl (geo_poly);
    gaiaFree(blob) ;

    cmd.clear() ;
}

namespace fs = boost::filesystem ;

bool OSMProcessor::processLandPolygon(const string &shp_file, const BBox &clip_box)
{
    cout << "Processing land polygon" << endl ;

    if ( !fs::exists(shp_file) ) {
        cerr << "cannot read land polygon: " << shp_file << endl ;
        return false ;
    }

    fs::path dir = fs::path(shp_file).parent_path() ;
    string file_name_prefix = fs::path(shp_file).stem().native() ;

    // open database file

    DBFHandle db_handle = DBFOpen( ( dir / (file_name_prefix + ".dbf") ).native().c_str(), "rb" );

    if ( !db_handle ) return false ;

    uint n_fields = DBFGetFieldCount(db_handle) ;

    vector<DBField> field_info ;

    for( uint i = 0 ; i<n_fields ; i++ ) {
        DBField fr ;

        fr.type_ = DBFGetFieldInfo( db_handle, i, fr.name_, &fr.width_, &fr.precision_);
        field_info.push_back(std::move(fr)) ;
    }

    SHPHandle shp_handle = SHPOpen( ( dir / file_name_prefix ).native().c_str(), "rb");

    int shp_entities, shp_geomtype ;
    SHPGetInfo( shp_handle, &shp_entities, &shp_geomtype, 0, 0) ;

    if ( shp_geomtype != SHPT_POLYGON ) {
        cerr << "no polygons found while reading land Shapefile: " << shp_file << endl ;
        return false ;
    }

    SQLite::Transaction trans(db_) ;

    stringstream sql ;
    sql <<  "INSERT INTO geom_polygons (geom, osm_id, zmin, zmax) SELECT ST_Multi(ST_Intersection(?, " ;
    sql <<  "ST_GeomFromText('POLYGON((" ;
    sql <<  clip_box.minx_ << ' ' << clip_box.miny_ << ',' ;
    sql <<  clip_box.maxx_ << ' ' << clip_box.miny_ << ',' ;
    sql <<  clip_box.maxx_ << ' ' << clip_box.maxy_ << ',' ;
    sql <<  clip_box.minx_ << ' ' << clip_box.maxy_ << ',' ;
    sql <<  clip_box.minx_ << ' ' << clip_box.miny_ << "))', 4326))) AS geom" ;
    sql <<  ",?, 0, 255 WHERE ST_IsValid(geom) AND geom NOT NULL;" ;

    string cmd_str = sql.str() ;
    SQLite::Statement cmd(db_, cmd_str) ;
    SQLite::Statement cmd_tags(db_, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, 0, 255)") ;

    osm_id_t id = 10000000000LL ;

    for( uint i=0 ; i<shp_entities ; i++ ) {

        SHPObject *obj = SHPReadObject( shp_handle, i );

        gaiaGeomCollPtr geom = gaiaAllocGeomColl() ;
        geom->Srid = 4326;

        geom->DeclaredType = GAIA_POLYGON;
        double *vx = obj->padfX, *vy = obj->padfY ;
        gaiaPolygonPtr gpoly = 0 ;

        for( int r = 0 ; r<obj->nParts ; r++ ) {
            uint count, last ;
            if ( r+1 < obj->nParts ) last = obj->panPartStart[r+1] ;
            else last = obj->nVertices ;

            count = last - obj->panPartStart[r] ;

            gaiaRingPtr ring = gaiaAllocRing (count);

            for (uint j=0 ; j<count ; j++) {
                gaiaSetPoint (ring->Coords, j, *vx++, *vy++);
            }

            if ( gpoly ) {
                gaiaInsertInteriorRing(gpoly, ring);
            }
            else {
                gpoly = gaiaInsertPolygonInGeomColl (geom, ring);
            }

        }

        gaiaOutBuffer buffer ;

        gaiaOutBufferInitialize(&buffer);
        gaiaOutWkt (&buffer,geom);
        SHPDestroyObject(obj) ;

        unsigned char *blob ;
        int blob_sz ;

        gaiaToSpatiaLiteBlobWkb (geom, &blob, &blob_sz) ;

        cmd.bindm(SQLite::Blob((const char *)blob, blob_sz), (osm_id_t)id) ;

        cmd.exec() ;
        cmd.clear() ;

        gaiaFree(blob);
        gaiaFreeGeomColl(geom);

        if ( db_.changes() ) { // if a non-null geometry resulted from intersection
            cmd_tags.bindm("natural", "nosea", (osm_id_t)id) ;
            cmd_tags.exec() ;
            cmd_tags.clear() ;

            ++id ;
        }
    }

    trans.commit() ;

    write_box_geometry(db_, clip_box, id) ;
    // write a sea polygon covering the map bounding box

    cmd_tags.bindm("natural", "sea", id) ;
    cmd_tags.exec() ; cmd_tags.clear() ;
    cmd_tags.bindm("level", "-5", id) ;
    cmd_tags.exec() ; cmd_tags.clear() ;

    return true ;
}

bool OSMProcessor::addDefaultLandPolygon(const BBox &clip_box)
{
    try {
        osm_id_t id = 10000000000LL ; // we have to take sure that somwhow this unique

        write_box_geometry(db_, clip_box, id) ;

        SQLite::Statement cmd_tags(db_, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, 0, 255)") ;

        cmd_tags.bindm("natural", "nosea", id) ;
        cmd_tags.exec() ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
}

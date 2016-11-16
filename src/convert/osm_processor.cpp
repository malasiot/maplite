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

        sql = "CREATE INDEX geom_" + desc + "_zmin_idx ON geom_" + desc + " (zmin)" ;
        db_.exec(sql) ;

        sql = "CREATE INDEX geom_" + desc + "_zmax_idx ON geom_" + desc + " (zmax)" ;
        db_.exec(sql) ;

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

OSMProcessor::OSMProcessor()
{
    boost::filesystem::path tmp_dir = boost::filesystem::temp_directory_path() ;
    boost::filesystem::path tmp_file = boost::filesystem::unique_path("%%%%%.sqlite");

    string spatialite_db_file = ( tmp_dir / tmp_file ).native() ;

    cout << spatialite_db_file << endl ;

    create(spatialite_db_file) ;
}

bool OSMProcessor::create(const std::string &name) {

    if ( boost::filesystem::exists(name) )
        boost::filesystem::remove(name);

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

            cmd.bindm(SQLite::Blob((const char *)blob, blob_size), way.id_, osm_way_t, zmin, zmax) ;
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

bool OSMProcessor::addMultiLineGeometry(SQLite::Statement &cmd, OSM::DocumentReader &reader, const std::vector<OSM::Way> &ways, osm_id_t id, osm_feature_t ftype, uint8_t zmin, uint8_t zmax)
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

        cmd.bindm(SQLite::Blob((const char *)blob, blob_size), poi.id_, osm_node_t, zmin, zmax) ;

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


bool OSMProcessor::addPolygonGeometry(SQLite::Statement &cmd, OSM::DocumentReader &reader, const OSM::Polygon &poly, osm_id_t id, osm_feature_t ftype,
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

void OSMProcessor::forAllPOITags(std::function<void (const string &, const string &)> f)
{
    for( const auto &lp: ntags_ ) {
        const Tag &t = lp.second ;
        f(t.key_, t.val_) ;
    }
}

void OSMProcessor::forAllWayTags(std::function<void (const string &, const string &)> f)
{
    for( const auto &lp: wtags_ ) {
        const Tag &t = lp.second ;
        f(t.key_, t.val_) ;
    }

    for( const auto &lp: rtags_ ) {
        const Tag &t = lp.second ;
        f(t.key_, t.val_) ;
    }
}

void OSMProcessor::getTags(osm_id_t id, osm_feature_t ft, uint8_t minz, uint8_t maxz, Dictionary &tags)
{
    typedef std::unordered_multimap<osm_id_t, Tag>::const_iterator mapit_t ;
    std::pair<mapit_t, mapit_t> range ;

    switch ( ft ) {
    case osm_node_t:
        range = ntags_.equal_range(id);
        break ;
    case osm_way_t:
        range = wtags_.equal_range(id);
        break ;
    case osm_relation_t:
        range = rtags_.equal_range(id);
        break ;
    }

    for_each( range.first, range.second, [&](const pair<osm_id_t, Tag> &kv) {
        const Tag &tag = kv.second ;

        uint8_t imin = std::max(tag.zmin_, minz) ;
        uint8_t imax = std::min(tag.zmax_, maxz) ;

        if ( imin <= imax ) tags.add(tag.key_, tag.val_) ;
    }) ;
}

void OSMProcessor::addTags(const TagWriteList &tags, osm_id_t id, osm_feature_t ft)
{
    for( const TagWriteAction &kv: tags.actions_)
        addTag(id, ft, kv.key_, kv.val_, kv.zoom_min_, kv.zoom_max_) ;
}

void OSMProcessor::addTag(osm_id_t id, osm_feature_t ftype, const string &key, const string &val, uint8_t zmin, uint8_t zmax)
{
    switch ( ftype ) {
    case osm_node_t:
        ntags_.emplace(std::make_pair(id, Tag{key, val, zmin, zmax})) ;
        break ;
    case osm_way_t:
        wtags_.emplace(std::make_pair(id, Tag{key, val, zmin, zmax})) ;
        break ;
    case osm_relation_t:
        rtags_.emplace(std::make_pair(id, Tag{key, val, zmin, zmax})) ;
        break ;

    }
}


bool OSMProcessor::processOsmFile(const string &osm_file, TagFilter &cfg)
{
    using namespace tag_filter ;

    // read files from memory and write to spatialite database

    uint8_t zmin, zmax ;

    try {

        OSM::DocumentReader reader ;

        cout << "Parsing file: " << osm_file << endl ;

        if ( !reader.read(osm_file) ) {
            cerr << "Error reading from " << osm_file << endl ;
            return false ;
        }

        // group all insertions into a transaction

        SQLite::Transaction trans(db_) ;


        SQLite::Statement cmd_pois(db_, insert_feature_sql("pois")) ;
        SQLite::Statement cmd_lines(db_, insert_feature_sql("lines", "ST_Multi(?)")) ;
        SQLite::Statement cmd_polygons(db_, insert_feature_sql("polygons", "ST_Multi(?)")) ;

        // POIs

        reader.forAllNodes([&] ( const OSM::Node &node )
        {
            if ( node.tags_.empty() ) return ;

            TagFilterContext ctx(node, &reader) ;
            if ( !cfg.match(ctx, zmin, zmax) ) return ;

            addPointGeometry(cmd_pois, node, zmin, zmax) ;
            addTags(ctx.tw_, node.id_, osm_node_t) ;
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

                if ( !chunks.empty() ) {
                    addMultiLineGeometry(cmd_lines, reader, chunks, relation.id_, osm_relation_t, zmin, zmax) ;
                    addTags(ctx.tw_, relation.id_, osm_relation_t) ;
                }
            }
            else if ( rel_type == "multipolygon"  ) {

                TagFilterContext ctx(relation.tags_, relation.id_,  TagFilterContext::Way) ;
                if ( !cfg.match(ctx, zmin, zmax) ) return ;

                OSM::Polygon polygon ;
                if ( !reader.makePolygonsFromRelation(relation, polygon) ) return ;

                if ( !polygon.rings_.empty() ) {
                    addPolygonGeometry(cmd_polygons, reader, polygon, relation.id_, osm_relation_t, zmin, zmax) ;
                    addTags(ctx.tw_, relation.id_, osm_relation_t) ;
                }
            }
        }) ;

        // ways

        reader.forAllWays([&] ( const OSM::Way &way )
        {
            if ( way.tags_.empty() ) return ;

            // match feature with filter rules

            TagFilterContext ctx(way, &reader) ;
            if ( !cfg.match(ctx, zmin, zmax) ) return ;

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

                addPolygonGeometry(cmd_polygons, reader, poly, way.id_, osm_way_t, zmin, zmax) ;
                addTags(ctx.tw_, way.id_, osm_way_t) ;
            }
            else {
                addLineGeometry(cmd_lines, reader, way, zmin, zmax) ;
                addTags(ctx.tw_, way.id_, osm_way_t) ;
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

static void write_box_geometry(SQLite::Connection &con, const BBox &box, osm_id_t id ) {

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

    cmd.bindm(SQLite::Blob((const char *)blob, blob_size), (osm_id_t)id, osm_way_t, 0, 255) ;
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
    sql <<  "INSERT INTO geom_polygons (geom, osm_id, osm_type, zmin, zmax) SELECT ST_Multi(ST_Intersection(?, " ;
    sql <<  "ST_GeomFromText('POLYGON((" ;
    sql <<  clip_box.minx_ << ' ' << clip_box.miny_ << ',' ;
    sql <<  clip_box.maxx_ << ' ' << clip_box.miny_ << ',' ;
    sql <<  clip_box.maxx_ << ' ' << clip_box.maxy_ << ',' ;
    sql <<  clip_box.minx_ << ' ' << clip_box.maxy_ << ',' ;
    sql <<  clip_box.minx_ << ' ' << clip_box.miny_ << "))', 4326))) AS geom" ;
    sql <<  ",?, ?, 0, 255 WHERE ST_IsValid(geom) AND geom NOT NULL;" ;

    string cmd_str = sql.str() ;
    SQLite::Statement cmd(db_, cmd_str) ;

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

        SHPDestroyObject(obj) ;

        unsigned char *blob ;
        int blob_sz ;

        gaiaToSpatiaLiteBlobWkb (geom, &blob, &blob_sz) ;

        cmd.bindm(SQLite::Blob((const char *)blob, blob_sz), (osm_id_t)id, (int)osm_way_t) ;

        cmd.exec() ;
        cmd.clear() ;

        gaiaFree(blob);
        gaiaFreeGeomColl(geom);

        if ( db_.changes() ) { // if a non-null geometry resulted from intersection
            addTag(id, osm_way_t, "natural", "nosea", 0, 255) ;
            ++id ;
        }
    }

    trans.commit() ;

    write_box_geometry(db_, clip_box, id) ;
    // write a sea polygon covering the map bounding box

    addTag(id, osm_way_t, "natural", "sea", 0, 255) ;
    addTag(id, osm_way_t, "level", "-5", 0, 255) ;

    return true ;
}

bool OSMProcessor::addDefaultLandPolygon(const BBox &clip_box)
{
    osm_id_t id = 10000000000LL ; // we have to take sure that somehow this is unique

    write_box_geometry(db_, clip_box, id) ;
    addTag(id, osm_way_t, "natural", "nosea", 0, 255) ;
}

static string make_bbox_query(const std::string &tableName, const BBox &bbox, int min_zoom,
                              int max_zoom, bool clip, double buffer, double tol, bool centroid)
{
    stringstream sql ;

    sql.precision(16) ;

    sql << "SELECT osm_id, osm_type, zmin, zmax, " ;

    if ( tol != 0.0 ) sql << "SimplifyPreserveTopology(" ;

    if ( clip ) {
        sql << "ST_ForceLHR(ST_Intersection(geom, ST_Transform(BuildMBR(" ;
        sql << bbox.minx_-buffer << ',' << bbox.miny_-buffer << ',' << bbox.maxx_+buffer << ',' << bbox.maxy_+buffer << "," << 3857 ;
        sql << "),4326)))" ;
    }
    else sql << "geom" ;

    if ( tol != 0 ) sql << ", " << tol << ")" ;

    sql << " AS _geom_ " ;
    if ( centroid ) sql << ", ST_Centroid(geom) " ;
    sql << " FROM " << tableName << " AS g ";

    sql << " WHERE " ;
    sql << "g.ROWID IN ( SELECT ROWID FROM SpatialIndex WHERE f_table_name='" << tableName << "' AND search_frame = ST_Transform(BuildMBR(" ;
    sql << bbox.minx_-buffer << ',' << bbox.miny_-buffer << ',' << bbox.maxx_+buffer << ',' << bbox.maxy_+buffer << "," << 3857 << "),4326)) " ;
    sql << "AND (( g.zmin BETWEEN " << (int)min_zoom << " AND " << max_zoom << " ) OR ( g.zmax BETWEEN " << min_zoom << " AND " << max_zoom << " ) OR ( g.zmin <= " << min_zoom << " AND g.zmax >= " << max_zoom << "))" ;
    sql << "AND _geom_ NOT NULL AND ST_IsValid(_geom_) " ;

    return sql.str() ;
}

bool OSMProcessor::forAllGeometries(const std::string &tableName, const BBox &bbox, uint8_t minz, uint8_t maxz,
                                    bool clip, double buffer, double tol, bool centroid,
                                    std::function<void (gaiaGeomCollPtr, osm_id_t, osm_feature_t, uint8_t, uint8_t, double, double)> f)
{
    try {
        // fetch geometries within bounding box and optionally do clipping, simplification and centroid computation

        string sql = make_bbox_query(tableName, bbox, minz, maxz, clip, buffer, tol, centroid) ;
        SQLite::Query q(db_, sql) ;

        for( const SQLite::Row &r: q.exec() ) {

            osm_id_t osm_id = r[0].as<osm_id_t>() ;
            osm_feature_t osm_type = static_cast<osm_feature_t>(r[1].as<int>()) ;

            uint8_t minz = r[2].as<int>() ;
            uint8_t maxz = r[3].as<int>() ;

            // get geometry
            SQLite::Blob blob = r[4].as<SQLite::Blob>() ;
            gaiaGeomCollPtr geom = gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)blob.data(), blob.size());

            // get centroid if requested
            double clat, clon ;
            if ( centroid ) {
                SQLite::Blob data = r[5].as<SQLite::Blob>() ;
                gaiaGeomCollPtr cg = gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)data.data(), data.size());

                clon = cg->FirstPoint->X ;
                clat = cg->FirstPoint->Y ;

                gaiaFreeGeomColl(cg) ;
            }

            // call the handler
            f(geom, osm_id, osm_type, minz, maxz, clat, clon) ;

            gaiaFreeGeomColl(geom) ;
        }
    }
    catch ( SQLite::Exception &e ) {
        return false ;
    }
}

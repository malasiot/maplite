#include "osm_processor.hpp"
#include "osm_memory_accessor.hpp"

#include <spatialite.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <shapefil.h>
#include <iomanip>

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
    SQLite::Session session(db_.get()) ;
    SQLite::Connection &con = session.handle() ;

    try {
        string sql ;

        sql = "CREATE TABLE geom_" + desc;
        sql += " (osm_id TEXT PRIMARY KEY, zmin INTEGER, zmax INTEGER)" ;

        SQLite::Command(con, sql).exec() ;

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

        SQLite::Command(con, sql).exec() ;

        // create spatial index

        con.exec("SELECT CreateSpatialIndex('geom_" + desc + "', 'geom');") ;

        return true ;
    }
    catch ( SQLite::Exception &e)
    {
        cerr << e.what() << endl ;
        return false ;
    }
}

bool OSMProcessor::createTagsTable()
{
    SQLite::Session session(db_.get()) ;
    SQLite::Connection &con = session.handle() ;

    try {
        string sql ;

        sql = "CREATE TABLE kv ";
        sql += "(key TEXT, val TEXT, osm_id TEXT, zoom_min INTEGER, zoom_max INTEGER);CREATE INDEX kv_index ON kv(osm_id)" ;

        SQLite::Command(con, sql).exec() ;

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
    sql += " (geom, osm_id, zmin, zmax) VALUES (" + geom_cmd + ", ?, ?, ?)";
    return sql ;
}

static void get_geometry_extent(SQLite::Connection &con, const string &desc, double &min_lat, double &min_lon, double &max_lat, double &max_lon) {
    string sql = "SELECT Extent(geom) from geom_" + desc ;
    SQLite::Query q(con, sql) ;

    SQLite::QueryResult res = q.exec() ;

    if ( res ) {
        int blob_size ;
        const char *data = res.getBlob(0, blob_size) ;

        if ( data == nullptr ) return ;

        gaiaGeomCollPtr geom = gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)data, blob_size);

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

    SQLite::Session session(db_.get()) ;
    SQLite::Connection &con = session.handle() ;

    get_geometry_extent(con, "pois", min_lat, min_lon, max_lat, max_lon) ;
    get_geometry_extent(con, "lines", min_lat, min_lon, max_lat, max_lon) ;
    get_geometry_extent(con, "polygons", min_lat, min_lon, max_lat, max_lon) ;

    return BBox(min_lon, min_lat, max_lon, max_lat) ;
}

bool OSMProcessor::create(const std::string &name) {

    if ( boost::filesystem::exists(name) )
        boost::filesystem::remove(name);

    db_.reset(new SQLite::Database(name, 16)) ;

    SQLite::Session session(db_.get()) ;
    SQLite::Connection &con = session.handle() ;

    try {
        con.exec("PRAGMA synchronous=NORMAL") ;
        con.exec("PRAGMA journal_mode=WAL") ;
        con.exec("SELECT InitSpatialMetadata(1);") ;
        con.exec("PRAGMA encoding=\"UTF-8\"") ;

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

bool OSMProcessor::addLineGeometry(SQLite::Command &cmd, OSM::DocumentAccessor &doc, const OSM::Way &way, uint8_t zmin, uint8_t zmax)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_line = gaiaAllocGeomColl();
        geo_line->Srid = 4326;
        geo_line->DeclaredType = GAIA_MULTILINESTRING ;

        gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_line, way.nodes_.size());

        vector<OSM::Node> nodes = doc.fetchNodes(way.nodes_) ;

        for(int j=0 ; j<nodes.size() ; j++)  {
            const OSM::Node &node = nodes[j] ;
            gaiaSetPoint (ls->Coords, j, node.lon_, node.lat_);
        }

        gaiaToSpatiaLiteBlobWkb (geo_line, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, (long long)way.id_) ;
        cmd.bind(3, (int)zmin) ;
        cmd.bind(4, (int)zmax) ;
        cmd.exec() ;

        gaiaFreeGeomColl (geo_line);
        free(blob) ;

        cmd.clear() ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}

bool OSMProcessor::addMultiLineGeometry(SQLite::Command &cmd, OSM::DocumentAccessor &doc, const std::vector<OSM::Way> &ways, int64_t id, uint8_t zmin, uint8_t zmax)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_mline = gaiaAllocGeomColl();
        geo_mline->Srid = 4326;
        geo_mline->DeclaredType = GAIA_MULTILINESTRING ;

        for( auto &way: ways ) {

            gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_mline, way.nodes_.size());

            vector<OSM::Node> nodes = doc.fetchNodes(way.nodes_) ;

            for(int j=0 ; j<nodes.size() ; j++)  {
                const OSM::Node &node = nodes[j] ;
                gaiaSetPoint (ls->Coords, j, node.lon_, node.lat_);
            }
        }

        gaiaToSpatiaLiteBlobWkb (geo_mline, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, (long long)id) ;
        cmd.bind(3, (int)zmin) ;
        cmd.bind(4, (int)zmax) ;

        cmd.exec() ;

        gaiaFreeGeomColl (geo_mline);
        free(blob) ;

        cmd.clear() ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}

bool OSMProcessor::addPointGeometry(SQLite::Command &cmd, const OSM::Node &poi, uint8_t zmin, uint8_t zmax)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_pt = gaiaAllocGeomColl();
        geo_pt->DeclaredType = GAIA_POINT ;
        geo_pt->Srid = 4326;

        gaiaAddPointToGeomColl (geo_pt, poi.lon_, poi.lat_);

        gaiaToSpatiaLiteBlobWkb (geo_pt, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, (long long)poi.id_) ;
        cmd.bind(3, (int)zmin) ;
        cmd.bind(4, (int)zmax) ;

        cmd.exec() ;

        gaiaFreeGeomColl (geo_pt);
        free(blob) ;

        cmd.clear() ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

}

bool OSMProcessor::addPolygonGeometry(SQLite::Command &cmd, OSM::DocumentAccessor &doc, const OSM::Polygon &poly, int64_t id,
                                      uint8_t zmin, uint8_t zmax)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_poly = gaiaAllocGeomColl();
        geo_poly->Srid = 4326;
        geo_poly->DeclaredType = GAIA_MULTIPOLYGON ;

        gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl (geo_poly, poly.rings_[0].nodes_.size(),
                poly.rings_.size() - 1);

        gaiaRingPtr er = g_poly->Exterior ;

        vector<OSM::Node> nodes = doc.fetchNodes(poly.rings_[0].nodes_) ;

        for(int j=0 ; j<nodes.size(); j++)  {
            const OSM::Node &node = nodes[j] ;
            gaiaSetPoint (er->Coords, j, node.lon_, node.lat_);
        }

        for( int i=1 ; i<poly.rings_.size() ; i++ ) {

            const OSM::Ring &ring = poly.rings_[i] ;
            gaiaRingPtr ir = gaiaAddInteriorRing(g_poly, i-1, ring.nodes_.size()) ;

            vector<OSM::Node> nodes = doc.fetchNodes(ring.nodes_) ;

            for(int j=0 ; j<nodes.size() ; j++)  {
                const OSM::Node &node = nodes[j] ;
                gaiaSetPoint (ir->Coords, j, node.lon_, node.lat_);
            }
        }
        /*
        gaiaOutBuffer wkt ;
        gaiaOutBufferInitialize (&wkt);
        gaiaOutWkt(&wkt, geo_poly) ;
*/
        gaiaToSpatiaLiteBlobWkb (geo_poly, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, (long long)id) ;
        cmd.bind(3, (int)zmin) ;
        cmd.bind(4, (int)zmax) ;

        cmd.exec() ;

        gaiaFreeGeomColl (geo_poly);
        free(blob) ;

        cmd.clear() ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}

bool OSMProcessor::addTags(SQLite::Command &cmd, const TagWriteList &tags, int64_t id)
{
    try {
        for( const TagWriteAction &kv: tags.actions_) {

            cmd.bind(1, kv.key_) ;
            cmd.bind(2, kv.val_) ;
            cmd.bind(3, (long long)id) ;
            cmd.bind(4, kv.zoom_min_) ;
            cmd.bind(5, kv.zoom_max_) ;

            cmd.exec() ;
            cmd.clear() ;
        }
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }
}


static bool member_of_multipolygon_or_boundary(const OSM::Way &way, OSM::DocumentAccessor &doc) {
    for( uint ridx: way.relations_ ) {
        OSM::Relation r = doc.fetchRelation(ridx)  ;
        string type = r.tags_.get("type") ;
        if ( type == "multipolygon" || type == "boundary" ) return true ;
    }
    return false ;
}


bool OSMProcessor::processOsmFile(const string &osm_file, TagFilter &cfg)
{
    using namespace tag_filter ;

    // read files from memory and write to spatialite database

    SQLite::Database &db = handle() ;

    SQLite::Session session(&db) ;
    SQLite::Connection &con = session.handle() ;

    uint8_t zmin, zmax ;

    try {

        SQLite::Command cmd_tags(con, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, ?, ?)") ;

        OSM::DocumentReader reader ;
        OSM::MemoryBasedAccessor doc ;

        cout << "Parsing file: " << osm_file << endl ;

        if ( !reader.read(osm_file, doc ) ) {
            cerr << "Error reading from " << osm_file << endl ;
            return false ;
        }

        // group all insertions into a transaction

        SQLite::Transaction trans(con) ;

        // POIs

        std::unique_ptr<OSM::NodeIteratorBase> nit = doc.nodes() ;

        while ( nit->isValid() )
        {
            const OSM::Node &node = nit->current() ;

            if ( node.tags_.empty() ) continue ;

            TagFilterContext ctx(node, node.id_, &doc) ;
            if ( !cfg.match(ctx, zmin, zmax) ) continue ;

            SQLite::Command cmd_poi(con, insert_feature_sql("pois")) ;

            addPointGeometry(cmd_poi, node, zmin, zmax) ;
            addTags(cmd_tags, ctx.tw_, node.id_) ;

            nit->next() ;
        }

        // relations of type route, merge ways into chunks

        std::unique_ptr<OSM::RelationIteratorBase> rit = doc.relations() ;

        while ( rit->isValid() )
        {
            const OSM::Relation &relation = rit->current() ;

            string rel_type = relation.tags_.get("type") ;

            TagFilterContext ctx(relation.tags_, relation.id_,  TagFilterContext::Relation) ;
            if ( !cfg.match(ctx, zmin, zmax) ) continue ;

            if ( rel_type == "route" ) {

                vector<OSM::Way> chunks ;
                if ( !OSM::DocumentReader::makeWaysFromRelation(doc, relation, chunks) ) continue ;

                SQLite::Command cmd_rel(con, insert_feature_sql("lines", "ST_Multi(?)")) ;

                if ( !chunks.empty() ) {
                    addMultiLineGeometry(cmd_rel, doc, chunks, relation.id_, zmin, zmax) ;
                    addTags(cmd_tags, ctx.tw_, relation.id_) ;
                }
            }
            else if ( rel_type == "multipolygon" || rel_type == "boundary" ) {

                OSM::Polygon polygon ;
                if ( !OSM::DocumentReader::makePolygonsFromRelation(doc, relation, polygon) ) continue ;

                SQLite::Command cmd_rel(con, insert_feature_sql("polygons", "ST_Multi(?)")) ;

                if ( !polygon.rings_.empty() ) {
                    addPolygonGeometry(cmd_rel, doc, polygon, relation.id_, zmin, zmax) ;
                    addTags(cmd_tags, ctx.tw_, relation.id_) ;
                }
            }

            rit->next() ;
        }

        // ways

        std::unique_ptr<OSM::WayIteratorBase> wit = doc.ways() ;

        while ( wit->isValid() )
        {
            const OSM::Way &way = wit->current() ;

            if ( way.tags_.empty() ) continue ;

            if ( member_of_multipolygon_or_boundary(way, doc) ) continue ;

            // match feature with filter rules

            TagFilterContext ctx(way, way.id_, &doc) ;
            if ( !cfg.match(ctx, zmin, zmax) ) continue ;

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

                SQLite::Command cmd_poly(con, insert_feature_sql("polygons", "CompressGeometry(ST_Multi(?))")) ;

                addPolygonGeometry(cmd_poly, doc, poly, way.id_, zmin, zmax) ;
                addTags(cmd_tags, ctx.tw_, way.id_) ;
            }
            else {
                SQLite::Command cmd_line(con, insert_feature_sql("lines", "CompressGeometry(ST_Multi(?))")) ;

                addLineGeometry(cmd_line, doc, way, zmin, zmax) ;
                addTags(cmd_tags, ctx.tw_, way.id_) ;
            }

            wit->next() ;
        }



        trans.commit() ;

        return true ;
    }
    catch ( SQLite::Exception & ) {
        return false ;
    }
}


struct DBField {
    DBFFieldType type_ ;
    int width_, precision_ ;
    char name_[12] ;
};

static bool write_box_geometry(SQLite::Connection &con, const BBox &box, const std::string &id ) {

    SQLite::Command cmd(con, insert_feature_sql("polygons", "ST_Multi(?)")) ;

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

    cmd.bind(1, blob, blob_size) ;
    cmd.bind(2, id) ;
    cmd.bind(3, 0) ;
    cmd.bind(4, 255) ;

    cmd.exec() ;

    gaiaFreeGeomColl (geo_poly);
    free(blob) ;

    cmd.clear() ;


}

namespace fs = boost::filesystem ;

bool OSMProcessor::processLandPolygon(const string &shp_file, const BBox &clip_box)
{
    cout << "Processing land polygon" << endl ;

    SQLite::Database &db = handle() ;

    SQLite::Session session(&db) ;
    SQLite::Connection &con = session.handle() ;

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

    SQLite::Transaction trans(con) ;

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
    SQLite::Command cmd(con, cmd_str) ;
    SQLite::Command cmd_tags(con, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, 0, 255)") ;

    uint64_t id = 1000000000 ;

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

        string sid = str(boost::format("%d") % id) ;

        unsigned char *blob ;
        int blob_sz ;

        gaiaToSpatiaLiteBlobWkb (geom, &blob, &blob_sz) ;

        cmd.bind(1, blob, blob_sz) ;
        cmd.bind(2, sid) ;

        cmd.exec() ;
        cmd.clear() ;

        free(blob);
        gaiaFreeGeomColl(geom);

        if ( con.changes() ) { // if a non-null geometry resulted from intersection
            cmd_tags.bind(1, "natural") ;
            cmd_tags.bind(2, "nosea") ;
            cmd_tags.bind(3, sid) ;

            cmd_tags.exec() ;
            cmd_tags.clear() ;

            ++id ;
        }


    }

    trans.commit() ;

    string sid = str(boost::format("%d") % id) ;
    write_box_geometry(con, clip_box, sid) ;
    // write a sea polygon covering the map bounding box

    cmd_tags.bind(1, "natural") ;
    cmd_tags.bind(2, "sea") ;
    cmd_tags.bind(3, sid) ;
    cmd_tags.exec() ;
    cmd_tags.clear() ;
    cmd_tags.bind(1, "level") ;
    cmd_tags.bind(2, "-5") ;
    cmd_tags.bind(3, sid) ;
    cmd_tags.exec() ;
    cmd_tags.clear() ;

    return true ;



}

bool OSMProcessor::addDefaultLandPolygon(const BBox &clip_box)
{
    SQLite::Database &db = handle() ;

    SQLite::Session session(&db) ;
    SQLite::Connection &con = session.handle() ;

    SQLite::Command cmd(con, insert_feature_sql("polygons", "?")) ;

    try {
        string id = "1000000000" ; // we have to take sure that somwhow this unique

        write_box_geometry(con, clip_box, id) ;
         /*
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_poly = gaiaAllocGeomColl();
        geo_poly->Srid = 4326;
        geo_poly->DeclaredType = GAIA_POLYGON ;

        gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl (geo_poly, 5, 0) ;
        gaiaRingPtr er = g_poly->Exterior ;

        gaiaSetPoint (er->Coords, 0, clip_box.minx_, clip_box.miny_);
        gaiaSetPoint (er->Coords, 1, clip_box.maxx_, clip_box.miny_);
        gaiaSetPoint (er->Coords, 2, clip_box.maxx_, clip_box.maxy_);
        gaiaSetPoint (er->Coords, 3, clip_box.minx_, clip_box.maxy_);
        gaiaSetPoint (er->Coords, 4, clip_box.minx_, clip_box.miny_);

        gaiaToSpatiaLiteBlobWkb (geo_poly, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, id) ;

        cmd.exec() ;

        gaiaFreeGeomColl (geo_poly);
        free(blob) ;

        cmd.clear() ;
*/
        SQLite::Command cmd_tags(con, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, 0, 255)") ;

        cmd_tags.bind(1, "natural") ;
        cmd_tags.bind(2, "nosea") ;
        cmd_tags.bind(3, id) ;

        cmd_tags.exec() ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }


}


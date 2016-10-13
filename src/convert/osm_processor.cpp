#include "osm_processor.hpp"

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
        sql += " (osm_id TEXT, zoom_min INTEGER, zoom_max INTEGER, PRIMARY KEY(osm_id))" ;

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
    sql += " (geom, osm_id, zoom_min, zoom_max) VALUES (" + geom_cmd + ", ?, ?, ?)";
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
    get_geometry_extent(con, "relations", min_lat, min_lon, max_lat, max_lon) ;
    get_geometry_extent(con, "polygons", min_lat, min_lon, max_lat, max_lon) ;

    return BBox(min_lon, min_lat, max_lon, max_lat) ;
}

bool OSMProcessor::create(const std::string &name) {

    if ( boost::filesystem::exists(name) )
        boost::filesystem::remove(name);

    db_.reset(new SQLite::Database(name)) ;

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
                 createGeometriesTable("polygons") &&
                 createGeometriesTable("relations")
                 ) ;
    }
    catch ( SQLite::Exception &e )
    {
        cerr << e.what()<< endl ;
        return false ;
    }
}

bool OSMProcessor::addLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const OSM::Way &way, uint8_t minz, uint8_t maxz)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_line = gaiaAllocGeomColl();
        geo_line->Srid = 4326;
        geo_line->DeclaredType = GAIA_LINESTRING ;

        gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_line, way.nodes_.size());

        for(int j=0 ; j<way.nodes_.size() ; j++)  {
            const OSM::Node &node = doc.nodes_[way.nodes_[j]] ;
            gaiaSetPoint (ls->Coords, j, node.lon_, node.lat_);
        }

        gaiaToSpatiaLiteBlobWkb (geo_line, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, way.id_) ;
        cmd.bind(3, (int)minz) ;
        cmd.bind(4, (int)maxz) ;

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

bool OSMProcessor::addMultiLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const std::vector<OSM::Way> &ways, const string &id,
                                        uint8_t minz, uint8_t maxz)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_mline = gaiaAllocGeomColl();
        geo_mline->Srid = 4326;
        geo_mline->DeclaredType = GAIA_MULTILINESTRING ;

        for( auto &way: ways ) {

            gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_mline, way.nodes_.size());

            for(int j=0 ; j<way.nodes_.size() ; j++)  {
                const OSM::Node &node = doc.nodes_[way.nodes_[j]] ;
                gaiaSetPoint (ls->Coords, j, node.lon_, node.lat_);
            }
        }

        gaiaToSpatiaLiteBlobWkb (geo_mline, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, id) ;
        cmd.bind(3, minz) ;
        cmd.bind(4, maxz) ;


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

bool OSMProcessor::addPOIGeometry(SQLite::Command &cmd, const OSM::Node &poi, uint8_t minz, uint8_t maxz)
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
        cmd.bind(2, poi.id_) ;
        cmd.bind(3, minz) ;
        cmd.bind(4, maxz) ;

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

bool OSMProcessor::addPolygonGeometry(SQLite::Command &cmd, const OSM::Document &doc, const OSM::Polygon &poly, const string &id,
                                      uint8_t minz, uint8_t maxz)
{
    try {
        unsigned char *blob;
        int blob_size;

        gaiaGeomCollPtr geo_poly = gaiaAllocGeomColl();
        geo_poly->Srid = 4326;
        geo_poly->DeclaredType = GAIA_POLYGON ;

        gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl (geo_poly,
                                                          poly.rings_[0].nodes_.size(),
                poly.rings_.size() - 1);

        gaiaRingPtr er = g_poly->Exterior ;

        for(int j=0 ; j<poly.rings_[0].nodes_.size() ; j++)  {
            const OSM::Node &node = doc.nodes_[poly.rings_[0].nodes_[j]] ;
            gaiaSetPoint (er->Coords, j, node.lon_, node.lat_);
        }

        for( int i=1 ; i<poly.rings_.size() ; i++ ) {

            const OSM::Ring &ring = poly.rings_[i] ;
            gaiaRingPtr ir = gaiaAddInteriorRing(g_poly, i-1, ring.nodes_.size()) ;

            for(int j=0 ; j<ring.nodes_.size() ; j++)  {
                const OSM::Node &node = doc.nodes_[ring.nodes_[j]] ;
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
        cmd.bind(2, id) ;
        cmd.bind(3, minz) ;
        cmd.bind(4, maxz) ;

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

bool OSMProcessor::addTags(SQLite::Command &cmd, const TagWriteList &tags, const string &id)
{
    try {
        for( const TagWriteAction &kv: tags.actions_) {

            cmd.bind(1, kv.key_) ;
            cmd.bind(2, kv.val_) ;
            cmd.bind(3, id) ;
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

// find the zoom range of attached tags

static bool normalize_tags(TagWriteList &tags, uint8_t &minz, uint8_t &maxz) {
    uint n_matched_tags = 0 ;
    minz = 255 ; maxz = 0;

    for( TagWriteAction &a: tags.actions_ ) {
        if ( !a.attached_ ) {
            minz = std::min(minz, a.zoom_min_) ;
            maxz = std::max(maxz, a.zoom_max_) ;
            n_matched_tags ++ ;
        }
    }

    if ( n_matched_tags == 0 ) return false ;

    for( TagWriteAction &a: tags.actions_ ) {
        if ( a.attached_ ) {
            a.zoom_min_ = minz ;
            a.zoom_max_ = maxz ;
        }
    }

    return true ;
}

using namespace OSM::Filter ;

bool OSMProcessor::processOsmFile(const string &osm_file, const FilterConfig &cfg)
{
    // read files from memory and write to spatialite database

    SQLite::Database &db = handle() ;

    SQLite::Session session(&db) ;
    SQLite::Connection &con = session.handle() ;

    uint8_t minz, maxz ;

    try {

        SQLite::Command cmd_tags(con, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, ?, ?)") ;

        OSM::Document doc ;

        cout << "Parsing file: " << osm_file << endl ;

        if ( !doc.read(osm_file ) ) {
            cerr << "Error reading from " << osm_file << endl ;
            return false ;
        }

        // group all insertions into a transaction

        SQLite::Transaction trans(con) ;

        // POIs

        for(int k=0 ; k<doc.nodes_.size() ; k++ )
        {
            auto node = doc.nodes_[k] ;

            if ( node.tags_.empty() ) continue ;

            OSM::Filter::Context ctx(node) ;

            TagWriteList tags ;
            bool cont = false;

            for( const RulePtr &rule: cfg.rules_ ) {
                if ( matchRule( rule, ctx, tags, cont) ) {
                    if ( !cont ) break ;
                }
            }

            if ( !normalize_tags(tags, minz, maxz) ) continue ;

            SQLite::Command cmd_poi(con, insert_feature_sql("pois")) ;

            addPOIGeometry(cmd_poi, node, minz, maxz) ;
            addTags(cmd_tags, tags, node.id_) ;
        }

        // ways

        for(int k=0 ; k<doc.ways_.size() ; k++ )
        {
            auto way = doc.ways_[k] ;

            if ( way.tags_.empty() ) continue ;


            // match feature with filter rules

            OSM::Filter::Context ctx(way) ;

            TagWriteList tags ;
            bool cont = false;

            for( const RulePtr &rule: cfg.rules_ ) {
                if ( matchRule( rule, ctx, tags, cont) ) {
                    if ( !cont ) break ;
                }
            }

            if ( !normalize_tags(tags, minz, maxz) ) continue ;

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

                addPolygonGeometry(cmd_poly, doc, poly, way.id_, minz, maxz) ;
                addTags(cmd_tags, tags, way.id_) ;
            }
            else {
                SQLite::Command cmd_line(con, insert_feature_sql("lines", "CompressGeometry(ST_Multi(?))")) ;

                addLineGeometry(cmd_line, doc, way, minz, maxz) ;
                addTags(cmd_tags, tags, way.id_) ;
            }
        }

        // relations of type route, merge ways into chunks

        for(int k=0 ; k<doc.relations_.size() ; k++ )
        {
            auto relation = doc.relations_[k] ;

            OSM::Filter::Context ctx(relation) ;

            TagWriteList tags ;
            bool cont = false;

            for( const RulePtr &rule: cfg.rules_ ) {
                if ( matchRule( rule, ctx, tags, cont) ) {
                    if ( !cont ) break ;
                }
            }

            if ( !normalize_tags(tags, minz, maxz) ) continue ;

            string rel_type = relation.tags_.get("type") ;

            if ( rel_type == "route" ) {
                vector<OSM::Way> chunks ;
                if ( !OSM::Document::makeWaysFromRelation(doc, relation, chunks) ) continue ;

                SQLite::Command cmd_rel(con, insert_feature_sql("relations", "CompressGeometry(ST_Multi(?))")) ;

                if ( !chunks.empty() ) {
                    addMultiLineGeometry(cmd_rel, doc, chunks, relation.id_, minz, maxz) ;
                    addTags(cmd_tags, tags, relation.id_) ;
                }
            }
            else if ( rel_type == "multipolygon" || rel_type == "boundary" ) {
                OSM::Polygon polygon ;
                if ( !OSM::Document::makePolygonsFromRelation(doc, relation, polygon) ) continue ;

                SQLite::Command cmd_rel(con, insert_feature_sql("polygons", "CompressGeometry(ST_Multi(?))")) ;

                if ( !polygon.rings_.empty() ) {
                    addPolygonGeometry(cmd_rel, doc, polygon, relation.id_, minz, maxz) ;
                    addTags(cmd_tags, tags, relation.id_) ;
                }
            }
        }

        trans.commit() ;

        return true ;
    }
    catch ( SQLite::Exception & ) {
        return false ;
    }
}

bool OSMProcessor::matchRule(const RulePtr &rule, Context &ctx, TagWriteList &tw, bool &cont)
{
    cont = false ;

    if ( !rule->condition_ || rule->condition_->eval(ctx).toBoolean() ) {

        bool rcont = true ;

        for ( const CommandPtr &cmd: rule->commands_->commands_ ) {
            if ( cmd->type() == Command::Conditional && rcont ) {
                RuleCommand *rc = dynamic_cast<RuleCommand *>(cmd.get());
                bool c = false ;
                if ( matchRule(rc->rule_, ctx, tw, c) ) {
                    if ( !c ) rcont = false ;
                }
            }
            else if ( cmd->type() == Command::Set ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());

                string val = rc->val_->eval(ctx).toString() ;

                if ( !val.empty() ) {
                    if ( ctx.has_tag(rc->tag_ ) )
                        ctx.tags_[rc->tag_] = val ;
                    else
                        ctx.tags_.add(rc->tag_, val) ;
                }
            }
            else if ( cmd->type() == Command::Add ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());
                string val = rc->val_->eval(ctx).toString() ;
                if ( !val.empty() ) ctx.tags_.add(rc->tag_, val) ;
            }
            else if ( cmd->type() == Command::Continue ) {
                cont = true ;
            }
            else if ( cmd->type() == Command::Delete ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());
                ctx.tags_.remove(rc->tag_) ;
            }
            else if ( cmd->type() == Command::WriteAll ) {
                WriteAllCommand *rc = dynamic_cast<WriteAllCommand *>(cmd.get());
                ZoomRange zr = rc->zoom_range_ ;
                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    string val = it.value() ;
                    tw.actions_.emplace_back(it.key(), val, zr.min_zoom_, zr.max_zoom_) ;
                    ++it ;
                }
            }
            else if ( cmd->type() == Command::Write ) {
                WriteCommand *rc = dynamic_cast<WriteCommand *>(cmd.get());
                ZoomRange zr = rc->zoom_range_ ;
                for( const TagDeclarationPtr &decl: rc->tags_.tags_ ) {
                    if ( ctx.has_tag(decl->tag_) ) {
                        if ( decl->val_ ) {
                            string val = decl->val_->eval(ctx).toString() ;
                            if ( !val.empty() )
                                tw.actions_.emplace_back(decl->tag_, val, zr.min_zoom_, zr.max_zoom_) ;
                        }
                        else
                            tw.actions_.emplace_back(decl->tag_, ctx.value(decl->tag_), zr.min_zoom_, zr.max_zoom_) ;
                    }
                }
            }
            else if ( cmd->type() == Command::Exclude ) {
                ExcludeCommand *rc = dynamic_cast<ExcludeCommand *>(cmd.get());

                set<string> exclude(rc->tags_.begin(), rc->tags_.end()) ;

                ZoomRange zr = rc->zoom_range_ ;
                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    if ( exclude.count(it.key()) == 0 )
                        tw.actions_.emplace_back(it.key(), it.value(), zr.min_zoom_, zr.max_zoom_) ;
                    ++it ;
                }
            }
            else if ( cmd->type() == Command::Attach ) {
                AttachCommand *rc = dynamic_cast<AttachCommand *>(cmd.get());

                set<string> tags(rc->tags_.tags_.begin(), rc->tags_.tags_.end()) ;

                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    if ( tags.count(it.key()) )
                        tw.actions_.emplace_back(it.key(), it.value(), 0, 255, true) ;
                    ++it ;
                }
            }
        }
        return true ;
    }
    else return false ;
}

struct DBField {
    DBFFieldType type_ ;
    int width_, precision_ ;
    char name_[12] ;
};
/*
string to_utf8( const string &src, const string &enc )
{
    std::vector<char> in_buf(src.begin(), src.end());
    char* src_ptr = &in_buf[0];
    size_t src_size = src.size();

    const size_t buf_size_ = 1024 ;
    const bool ignore_error_ = true ;

    std::vector<char> buf(buf_size_);
    std::string dst;

    iconv_t cd = iconv_open( "UTF-8", enc.c_str() );

    if ( cd != (iconv_t)-1 ) {

        while ( 0 < src_size ) {

            char* dst_ptr = &buf[0];
            size_t dst_size = buf.size();
            size_t res = ::iconv(cd, &src_ptr, &src_size, &dst_ptr, &dst_size);

            if (res == (size_t)-1) {
                if (errno == E2BIG)  ;
                else if ( ignore_error_ ) {
                    // skip character
                    ++src_ptr;
                    --src_size;
                } else {
                    break ;
                }
            }
            dst.append(&buf[0], buf.size() - dst_size);
        }

        iconv_close( cd );
    }

    return  dst ;
}
*/
static void parse_record(DBFHandle db_handle, int index, const vector<DBField> &fields, Dictionary &dict, const string &enc) {
    for( uint i=0 ;i<fields.size() ; i++ ) {
        const DBField &f = fields[i] ;

        stringstream fstr ;
        if ( f.type_ == FTDouble ) {
            double val = DBFReadDoubleAttribute(db_handle, index, i) ;
            fstr << std::setiosflags(ios::fixed) << val ;
        }
        else if ( f.type_ == FTInteger ) {
            int val = DBFReadIntegerAttribute(db_handle, index, i) ;
            fstr << val ;
        }
        else if ( f.type_ == FTString ) {
            const char *p = DBFReadStringAttribute(db_handle, index, i) ;
    //        fstr << to_utf8(p, enc) ;
        }

        string sval = fstr.str() ;

        if ( !sval.empty() )
            dict.add(f.name_, sval) ;
    }
}

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
    sql <<  "INSERT INTO geom_polygons (geom, osm_id) SELECT ST_Multi(ST_Intersection(?, " ;
    sql <<  "ST_GeomFromText('POLYGON((" ;
    sql <<  clip_box.minx_ << ' ' << clip_box.miny_ << ',' ;
    sql <<  clip_box.maxx_ << ' ' << clip_box.miny_ << ',' ;
    sql <<  clip_box.maxx_ << ' ' << clip_box.maxy_ << ',' ;
    sql <<  clip_box.minx_ << ' ' << clip_box.maxy_ << ',' ;
    sql <<  clip_box.minx_ << ' ' << clip_box.miny_ << "))', 4326))) AS geom" ;
    sql <<  ",? WHERE ST_IsValid(geom) AND geom NOT NULL;" ;

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


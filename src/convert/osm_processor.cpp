#include "osm_processor.hpp"

#include <spatialite.h>
#include <boost/filesystem.hpp>

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
        sql += " (gid INTEGER PRIMARY KEY AUTOINCREMENT, osm_id TEXT)" ;

        SQLite::Command(con, sql).exec() ;

        // Add geometry column

        string geom_type ;

        if ( desc == "lines" )
            geom_type = "LINESTRING" ;
        else if ( desc == "polygons")
            geom_type = "POLYGON" ;
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
        sql += "(gid INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, val TEXT, osm_id TEXT, zoom_min INTEGER, zoom_max INTEGER)" ;

        SQLite::Command(con, sql).exec() ;

        return true ;
    }
    catch ( SQLite::Exception &e)
    {
        cerr << e.what() << endl ;
        return false ;
    }
}


string OSMProcessor::insertFeatureSQL(const string &desc, const string &geomCmd )
{
    string sql ;

    sql = "INSERT INTO geom_" + desc ;
    sql += " (geom, osm_id) VALUES (?, ?)";
    return sql ;
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

        return ( createGeometriesTable("lines") &&
                 createGeometriesTable("pois") &&
                 createGeometriesTable("polygons") &&
                 createGeometriesTable("relations") &&
                 createTagsTable() ) ;
    }
    catch ( SQLite::Exception &e )
    {
        cerr << e.what()<< endl ;
        return false ;
    }



}

bool OSMProcessor::addLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const OSM::Way &way)
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

bool OSMProcessor::addMultiLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const std::vector<OSM::Way> &ways, const string &id)
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

bool OSMProcessor::addPOIGeometry(SQLite::Command &cmd, const OSM::Node &poi)
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

bool OSMProcessor::addPolygonGeometry(SQLite::Command &cmd, const OSM::Document &doc, const OSM::Polygon &poly, const string &id)
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

using namespace OSM::Filter ;

bool OSMProcessor::processOsmFiles(const vector<string> &osmFiles, const FilterConfig &cfg)
{
    // read files from memory and write to spatialite database

    SQLite::Database &db = handle() ;

    SQLite::Session session(&db) ;
    SQLite::Connection &con = session.handle() ;

    SQLite::Command cmd_tags(con, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, ?, ?)") ;

    for(int i=0 ; i<osmFiles.size() ; i++ ) {

        OSM::Document doc ;

        cout << "Reading file: " << osmFiles[i] << endl ;

        if ( !doc.read(osmFiles[i]) )
        {
            cerr << "Error reading from " << osmFiles[i] << endl ;
            continue ;
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

            if ( tags.actions_.empty() ) continue ;

            SQLite::Command cmd_poi(con, insertFeatureSQL("pois")) ;

            addPOIGeometry(cmd_poi, node) ;
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

            if ( tags.actions_.empty() ) continue ;

            // deal with closed ways, potential polygon geometries (areas) are those indicated by area tag or those other than highway, barrier and contour

            if ( way.nodes_.front() == way.nodes_.back() &&
                ( way.tags_.get("area") == "yes" ) ||
                 ( !way.tags_.contains("highway") &&
                   !way.tags_.contains("barrier") &&
                   !way.tags_.contains("contour") ) ) {

                OSM::Polygon poly ;

                OSM::Ring ring ;
                ring.nodes_.insert(ring.nodes_.end(), way.nodes_.begin(), way.nodes_.end()) ;
                poly.rings_.push_back(ring) ;

                SQLite::Command cmd_poly(con, insertFeatureSQL("polygons", "CompressGeometry(?)")) ;

                addPolygonGeometry(cmd_poly, doc, poly, way.id_) ;
                addTags(cmd_tags, tags, way.id_) ;
            }
            else {
               SQLite::Command cmd_line(con, insertFeatureSQL("lines", "CompressGeometry(?)")) ;

                addLineGeometry(cmd_line, doc, way) ;
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

            if ( tags.actions_.empty() ) continue ;

            string rel_type = relation.tags_.get("type") ;

            if ( rel_type == "route" ) {
                vector<OSM::Way> chunks ;
                if ( !OSM::Document::makeWaysFromRelation(doc, relation, chunks) ) continue ;

                SQLite::Command cmd_rel(con, insertFeatureSQL("relations", "CompressGeometry(ST_Multi(?))")) ;

                addMultiLineGeometry(cmd_rel, doc, chunks, relation.id_) ;
                addTags(cmd_tags, tags, relation.id_) ;
            }
            else if ( rel_type == "multipolygon" || rel_type == "boundary" ) {
                OSM::Polygon polygon ;
                if ( !OSM::Document::makePolygonsFromRelation(doc, relation, polygon) ) continue ;

                SQLite::Command cmd_rel(con, insertFeatureSQL("polygons", "CompressGeometry(?)")) ;

                addPolygonGeometry(cmd_rel, doc, polygon, relation.id_) ;
                addTags(cmd_tags, tags, relation.id_) ;
            }
        }

        trans.commit() ;
    }


    return true ;

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

                if ( ctx.has_tag(rc->tag_ ) )
                    ctx.tags_[rc->tag_] = rc->val_->eval(ctx).toString() ;
                else
                    ctx.tags_.add(rc->tag_, rc->val_->eval(ctx).toString()) ;
            }
            else if ( cmd->type() == Command::Add ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());
                ctx.tags_.add(rc->tag_, rc->val_->eval(ctx).toString()) ;
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
                    tw.actions_.emplace_back(it.key(), it.value(), zr.min_zoom_, zr.max_zoom_) ;
                    ++it ;
                }
            }
            else if ( cmd->type() == Command::Write ) {
                WriteCommand *rc = dynamic_cast<WriteCommand *>(cmd.get());
                ZoomRange zr = rc->zoom_range_ ;
                for( const TagDeclarationPtr &decl: rc->tags_.tags_ ) {
                    if ( ctx.has_tag(decl->tag_) ) {
                        if ( decl->val_ )
                            tw.actions_.emplace_back(decl->tag_, decl->val_->eval(ctx).toString(), zr.min_zoom_, zr.max_zoom_) ;
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
        }
        return true ;
    }
    else return false ;
}

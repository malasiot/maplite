#include "map_file.hpp"

#include <spatialite.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

using namespace std;

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


MapFile::MapFile():  db_(0), geom_column_name_("geom") {}

MapFile::~MapFile() {
    delete db_ ;
}

bool MapFile::create(const std::string &name) {

    if ( boost::filesystem::exists(name) )
        boost::filesystem::remove(name);

    db_ = new SQLite::Database(name) ;

    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {
        con.exec("PRAGMA synchronous=NORMAL") ;
        con.exec("PRAGMA journal_mode=WAL") ;
        con.exec("SELECT InitSpatialMetadata(1);") ;
        con.exec("PRAGMA encoding=\"UTF-8\"") ;

        return true ;
    }
    catch ( SQLite::Exception & )
    {

        return false ;
    }

}

bool MapFile::hasLayer(const std::string &layerName) const
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {
        string sql = "SELECT count(*) FROM sqlite_master WHERE sqlite_master.type='table' AND sqlite_master.name=?;" ;

        SQLite::Query q(con, sql) ;

        q.bind(layerName) ;

        SQLite::QueryResult res = q.exec() ;

        return ( res && res.get<int>(0) ) ;
    }
    catch ( SQLite::Exception & )
    {
        return false ;
    }

}

bool MapFile::createLayerTable(const std::string &layerName, const string &layerType, const string &layerSrid)
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {
        bool has_layer = hasLayer(layerName) ;
        if ( has_layer ) return false ;

        string sql ;

        sql = "CREATE TABLE ";
        sql +=  layerName + "(gid INTEGER PRIMARY KEY AUTOINCREMENT, tags TEXT)" ;

        SQLite::Command(con, sql).exec() ;

        // Add geometry column

        sql = "SELECT AddGeometryColumn( '"  ;
        sql += layerName ;
        sql += "', '" + geom_column_name_ +"', " + layerSrid + "," ;

        if ( layerType == "points" )
            sql += "'POINT', 2);" ;
        else if ( layerType == "lines" )
            sql +="'LINESTRING', 2);" ;
        else if ( layerType == "polygons" )
            sql += "'POLYGON', 2);" ;

        SQLite::Command(con, sql).exec() ;

        // create spatial index

        con.exec("SELECT CreateSpatialIndex('%q', '%q');", layerName.c_str(), geom_column_name_.c_str()) ;

        return true ;
    }
    catch ( SQLite::Exception &e)
    {
        cerr << e.what() << endl ;
        return false ;
    }
}


string MapFile::insertFeatureSQL(const string &layerName, const string &geomCmd )
{
    string sql ;

    sql = "INSERT INTO " ;
    sql += layerName ;
    sql += "(" + geom_column_name_ ;
    sql += ",tags" ;

    sql += ") VALUES (" + geomCmd + ",?)";
    return sql ;
}


static string makeBBoxQuery(const std::string &tableName, const std::string &geomColumn,
                            const BBox &bbox, double tol)
{
    stringstream sql ;

    sql.precision(16) ;

    sql << "SELECT tags," ;

    if ( tol != 0.0 ) {
        sql << "SimplifyPreserveTopology(ST_ForceLHR(ST_Intersection(" << geomColumn << ",BuildMBR(" ;
    }
    else
        sql << "ST_ForceLHR(ST_Intersection(" << geomColumn << ",BuildMBR(" ;

    sql << bbox.minx_ << ',' << bbox.miny_ << ',' << bbox.maxx_ << ',' << bbox.maxy_ << "," << bbox.srid_ ;
    sql << ")))" ;


    if ( tol != 0 )
        sql << ", " << tol << ")" ;

    sql << " AS _geom_ FROM " << tableName << " AS __table__";

    sql << " WHERE " ;

    sql << "__table__.ROWID IN ( SELECT ROWID FROM SpatialIndex WHERE f_table_name='" << tableName << "' AND search_frame = BuildMBR(" ;
    sql << bbox.minx_ << ',' << bbox.miny_ << ',' << bbox.maxx_ << ',' << bbox.maxy_ << "," << bbox.srid_ << ")) AND _geom_ NOT NULL" ;

    return sql.str() ;
}

void MapFile::deserializeTags(const string &tags, Dictionary &attr)
{
    using boost::tokenizer;
    using boost::escaped_list_separator;

    typedef tokenizer<escaped_list_separator<char> > stokenizer;

    stokenizer tok(tags, escaped_list_separator<char>("\\", "@;", "\"'"));

    vector<string> tokens ;

    for( stokenizer::iterator beg = tok.begin(); beg!=tok.end(); ++beg)
        if ( !beg->empty() ) tokens.push_back(*beg) ;

    for(uint i=0 ; i<tokens.size() ; i+=2)
        attr.add(tokens[i], tokens[i+1]) ;
}

string escape_tag(const string &src) {

    string out ;

    for( auto c: src ) {
      if ( c == '@' ) out += "\\@" ;
      else if ( c == ';' ) out += "\\;" ;
      else out += c ;
    }

    return out ;
}

string MapFile::serializeTags(const Dictionary &tags)
{
    DictionaryIterator it(tags) ;

    string res ;

    while( it ) {
        res += escape_tag(it.key()) + '@' + escape_tag(it.value()) + ';' ;
        ++it ;
    }

    return res ;
}


bool MapFile::queryTile(const MapConfig &cfg, VectorTileWriter &tile) const
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    BBox box = tile.box(16);

    bool has_data = false ;

    for ( const Layer &layer: cfg.layers_ ) {

        if ( !hasLayer( layer.name_) ) continue ;

        bool zoom_matches = false ;
        double stol = 0 ;

        for( auto iv: layer.zr_.intervals_) {
            if ( ( iv.min_zoom_ == -1 && tile.z() <= iv.max_zoom_ ) ||
                 ( iv.max_zoom_ == -1 && tile.z() >= iv.min_zoom_ ) ||
                 ( tile.z() >= iv.min_zoom_ && tile.z() <= iv.max_zoom_ ) ) {
                zoom_matches = true ;
                stol = iv.simplify_threshold_ ;
            }
        }

        if ( !zoom_matches ) continue ; // layer zoom range does not match


        string sql = makeBBoxQuery(layer.name_, geom_column_name_,  box, stol) ;

        try {

            SQLite::Query q(con, sql) ;

            SQLite::QueryResult res = q.exec() ;

            if ( !res ) continue ;

 //           tile.beginLayer(layer.name_) ;
            has_data = true ;

            while ( res )
            {
                int buf_size ;
                const char *data = res.getBlob("_geom_", buf_size) ;

                gaiaGeomCollPtr geom = gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)data, buf_size);
                string tags = res.get<string>("tags") ;

                Dictionary attr ;

                deserializeTags(tags, attr) ;
//                tile.encodeFeatures(geom, attr) ;

                res.next() ;
            }

 //           tile.endLayer() ;
        }
        catch ( SQLite::Exception &e )
        {
            cout << e.what() << endl ;
            return false ;
        }


    }

    return has_data ;
}



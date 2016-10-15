#include "MapFile.h"
#include "QueryProcessor.h"

#include "XmlDocument.h"

#include <spatialite.h>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <proj_api.h>
#include <zlib.h>

using namespace std;
namespace fs = boost::filesystem ;

class SpatialLiteSingleton
{
    public:

    static SpatialLiteSingleton instance_;

    static SpatialLiteSingleton& instance() {
            return instance_;
    }

private:

    SpatialLiteSingleton () {
        spatialite_init(true);
    };

    ~SpatialLiteSingleton () {
        spatialite_cleanup();
    }

    SpatialLiteSingleton( SpatialLiteSingleton const & );

    void operator = ( SpatialLiteSingleton const & );


};

SpatialLiteSingleton SpatialLiteSingleton::instance_ ;


class ProjectionCache {
public:
    ProjectionCache(const MapFile &file): mf(file) {

    }

    Projection *query(int srid) {

        ContainerType::const_iterator c = projs.find(srid) ;

        if ( c != projs.end()  )
            return c->second ;
        else
        {
            string prj4src = mf.getProj4SRID(srid) ;
            if ( prj4src.empty() ) return 0 ;
            else
            {

                Projection *p = new Projection ;
                if ( !p->init(prj4src) ) {
                    delete p ;
                    return 0 ;
                }


                projs.insert(make_pair(srid, p)) ;

                return p ;

            }

        }

        return 0 ;

    }

    ~ProjectionCache()
    {
         ContainerType::const_iterator i = projs.begin() ;

         for( ; i != projs.end() ; ++i )
             delete i->second ;

    }

private:

    const MapFile &mf ;

    typedef map<int, Projection *> ContainerType ;
    ContainerType projs ;

};

#define MAX_TILE_CACHE_BYTES 1024*1024*64

MapFile::MapFile(): pj_cache_(new ProjectionCache(*this)), tileCache(MAX_TILE_CACHE_BYTES), db_(0) {}

MapFile::~MapFile() {
    delete db_ ;
    delete pj_cache_ ;
}

bool MapFile::open(const string &filePath)
{
     if ( !boost::filesystem::exists(filePath) ) return false ;

     if ( !db_ ) {
         db_ = new SQLite::Database(filePath) ;
        fileName = filePath ;
     }

     if ( !readMetaData() ) return false ;

     return true ;
}


string MapFile::getLayerGeometryColumn(const string &layerName) const
{
    vector<string> columns ;
    getLayerGeometryColumns(layerName, columns) ;

    if ( columns.empty() ) return string() ;
    else return columns[0] ;
}

void MapFile::getLayerGeometryColumns(const string &layerName, vector<string> &names) const
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {
        string sql = "SELECT f_geometry_column FROM geometry_columns WHERE f_table_name=?" ;

        SQLite::Query q(con, sql) ;
        q.bind(layerName) ;

        SQLite::QueryResult res = q.exec() ;

        while ( res ) {
            names.push_back(res.get<string>(0)) ;
            res.next() ;
        }
    }
    catch ( SQLite::Exception &e )
    {

    }

}

bool MapFile::getLayerInfo(const string &layerName, LayerInfo &info)
{
    if ( layerCache.count(layerName) )
    {
        info = layerCache[layerName] ;
        return true ;
    }
    else {

        SQLite::Session session(db_) ;
        SQLite::Connection &con = session.handle() ;

        try {
            info.name = layerName ;

            set<string> dict_columns ;

            {

                string sql = "SELECT column_name FROM __dictionary_metadata WHERE table_name = '" + layerName + "';" ;

                SQLite::Query q(con, sql) ;

                SQLite::QueryResult res = q.exec() ;

                while ( res ) {
                    dict_columns.insert(res.get<string>("column_name")) ;
                    res.next() ;
                }
            }

            // test if we have a raster
            {
                string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + layerName + "_rasters'" ;

                SQLite::Query q(con, sql) ;

                SQLite::QueryResult res = q.exec() ;

                if ( res ) {
                    info.type = "rasterlite" ;
                    layerCache.insert(std::make_pair(layerName, info)) ;
                    return true ;
                }
            }

            // load vector layer metadata

            vector<string> geom_columns ;
            getLayerGeometryColumns(layerName, geom_columns) ;

            if ( geom_columns.empty() ) return false ;

            info.geomColumn = geom_columns[0] ;

            info.attrs.reset(new AttributeCollection) ;

            string sql = "PRAGMA table_info('" + layerName + "');" ;

            SQLite::Query q(con, sql) ;

            SQLite::QueryResult res = q.exec() ;

            while ( res )
            {
                string name_ = res.get<string>("name") ;
                string type_ = res.get<string>("type") ;
                int pk_ = res.get<int>("pk") ;

                if ( type_ == "INTEGER" )
                {
                    if ( dict_columns.count(name_) == 0 )
                        info.attrs->add(name_, AttributeDescriptor::Integer) ;
                    else
                        info.attrs->add(name_, AttributeDescriptor::DictionaryIndex) ;
                }
                else if ( type_ == "TEXT" )
                    info.attrs->add(name_, AttributeDescriptor::Text) ;
                else if ( type_ == "REAL" )
                    info.attrs->add(name_, AttributeDescriptor::Real) ;

                res.next() ;
            }

            layerCache.insert(std::make_pair(layerName, info)) ;
            return true ;
        }
        catch ( SQLite::Exception &e )
        {
            cerr << e.what() << endl ;
            return false ;
        }
    }


}

string MapFile::getProj4SRID(int srid) const
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {

        string sql = "SELECT proj4text FROM spatial_ref_sys WHERE srid=? LIMIT 1;" ;

        SQLite::Query q(con, sql) ;

        q.bind(srid) ;

        SQLite::QueryResult res = q.exec() ;

        if ( res )
            return res.get<string>(0) ;

    }
    catch ( SQLite::Exception &e )
    {
        return string() ;
    }

}


void MapFile::getLayerAttributes(const std::string &layerName, vector<string> &names) const
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {
        string sql = "PRAGMA table_info('" + layerName + "');" ;

        SQLite::Query q(con, sql) ;

        SQLite::QueryResult res = q.exec() ;

        while ( res )
        {
            string name_ = res.get<string>("name") ;
            string type_ = res.get<string>("type") ;
            int pk_ = res.get<int>("pk") ;

            if ( type_ == "INTEGER" || type_ == "TEXT" || type_ == "REAL" )
                names.push_back(name_) ;

            res.next() ;
        }

    }
    catch ( SQLite::Exception &e )
    {

    }

}

bool MapFile::hasTable(const std::string &tableName) const
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {
        string sql = "SELECT count(*) FROM sqlite_master WHERE sqlite_master.type='table' AND sqlite_master.name=?;" ;

        SQLite::Query q(con, sql) ;

        q.bind(tableName) ;

        SQLite::QueryResult res = q.exec() ;

        return ( res && res.get<int>(0) ) ;
    }
    catch ( SQLite::Exception & )
    {
        return false ;
    }

}

bool MapFile::hasLayer(const std::string &layerName)
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

static string read_resource( const char *src, size_t sz)
{
    string bytes ;

    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if ( inflateInit(&zs) != Z_OK ) return string() ;

    zs.next_in = (Bytef*)src;
    zs.avail_in = sz;

    int ret ;
    char outbuffer[32768];

    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if ( bytes.size() < zs.total_out ) {
            bytes.append(outbuffer, zs.total_out - bytes.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        return string() ;
    }

    return bytes;

}

string MapFile::readResourceFile(const string &path) const
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    if ( ! hasTable("__resources") ) return string() ;

    string sql = "SELECT data FROM __resources WHERE name='" ;
    sql += path ;
    sql += "'" ;

    SQLite::Query stmt(con, sql) ;

    SQLite::QueryResult res = stmt.exec() ;

    if ( res )
    {
        int bs ;
        const char *blob = res.getBlob(0, bs) ;
        return read_resource(blob, bs) ;

    }
    else return string() ;

}

bool MapFile::readMetaData()
{
    string data = readResourceFile("map.xml") ;
    if ( data.empty() ) return false ;

    XmlDocument doc ;

    istringstream strm(data) ;

    if ( !doc.load(strm) ) return false ;

    if ( !desc_.parseXml(doc) ) return false ;

    return true ;


}


bool BBox::transform(int target_srid, BBox &q, ProjectionCache &prj) const
{
    Projection *src_prj, *dst_prj ;
    if ( ( src_prj = prj.query(srid) ) == 0 ) return false ;
    if ( ( dst_prj = prj.query(target_srid) ) == 0 ) return false ;

    src_prj->inverse(minx, miny, q.minx, q.miny) ;
    dst_prj->forward(q.minx, q.miny) ;

    src_prj->inverse(maxx, maxy, q.maxx, q.maxy) ;
    dst_prj->forward(q.maxx, q.maxy) ;

    q.srid = target_srid ;

    return true ;
}


bool MapDescription::parseXml(const XmlDocument &doc)
{
    const XmlNode *node = doc.root();
    if ( node->nodeName() != "Map") return false ;

    const XmlElement *map_elem = node->toElement() ;

    name_ = map_elem->attribute("name") ;

    if ( name_.empty() ) return false ;

    XmlElement *pNode ;

    XML_FOREACH_CHILD_ELEMENT(node, pNode)
    {
        const string &tag = pNode->tag() ;

        if ( tag == "Description" )
            description_ = pNode->text() ;
        else if ( tag == "Attribution" )
            attribution_ = pNode->text() ;
        else if ( tag == "Layers" )
            layers_ = pNode->text().c_str() ;
        else if ( tag == "Styles" )
            styles_ = pNode->text().c_str() ;
        else if ( tag == "Background" )
            bg_color_ = pNode->text().c_str() ;
        else if ( tag == "ImageFormat" )
            image_format_ = pNode->text().c_str() ;
        else if ( tag == "MinZ")
        {
            try {
                min_z_ = boost::lexical_cast<size_t>(pNode->text()) ;
            }
            catch ( ... )
            {
                min_z_ = 10 ;
            }
        }
        else if ( tag == "MaxZ")
        {
            try {
                max_z_ = boost::lexical_cast<size_t>(pNode->text()) ; ;
            }
            catch ( ... )
            {
                max_z_ = 25 ;
            }
        }
        else if ( tag == "Url")
            url_ = pNode->text() ;
        else if ( tag == "TileSize")
        {
            try {
                tile_size_ = boost::lexical_cast<size_t>(pNode->text()) ;
            }
            catch ( ... )
            {
                tile_size_ = 256 ;
            }
        }
        else if ( tag == "Resources")  {
            resource_path_ = pNode->text() ;
        }
        else if ( tag == "BBox" ) {

            vector<string> tokens;

            string src = pNode->text() ;

            boost::split(tokens, src, boost::is_any_of(" ,"), boost::token_compress_on);

            if ( tokens.size() < 4 ) return false ;

            try {
                bbox_.minx = boost::lexical_cast<double>(tokens[0]) ;
                bbox_.miny = boost::lexical_cast<double>(tokens[1]) ;
                bbox_.maxx = boost::lexical_cast<double>(tokens[2]) ;
                bbox_.maxy = boost::lexical_cast<double>(tokens[3]) ;
            }
            catch ( ... )
            {
                cout << "error" << endl ;
                return false ;
            }

            try {
                bbox_.srid = boost::lexical_cast<size_t>(pNode->attribute("srid", "4326")) ;
            }
            catch ( ... )
            {
                bbox_.srid = 4326 ;
            }

        }
    }

   return true ;
}

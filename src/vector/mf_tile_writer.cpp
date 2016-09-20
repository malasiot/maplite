#include "mf_tile_writer.hpp"


#include <boost/filesystem.hpp>
#include <fstream>

namespace fs = boost::filesystem ;

using namespace std ;

MBTileWriter::MBTileWriter(const std::string &fileName): tileset_(fileName)
{
    if ( !fs::is_directory(fileName) ) {
        if ( fs::exists(fileName) ) fs::remove(fileName);

        db_.reset(new SQLite::Database(fileName)) ;

        SQLite::Session session(db_.get()) ;
        SQLite::Connection &con = session.handle() ;

        con.exec("CREATE TABLE metadata (name text, value text, UNIQUE(name));") ;
        con.exec("CREATE TABLE tiles (zoom_level integer, tile_column integer, tile_row integer, tile_data blob, UNIQUE (zoom_level, tile_column, tile_row));") ;
    }
    else {
        if ( !fs::exists(tileset_) )
            fs::create_directory(tileset_) ;
    }
}


bool MBTileWriter::writeMetaData(const std::string &name, const std::string &val)
{
    assert(db_) ;

    SQLite::Session session(db_.get()) ;
    SQLite::Connection &con = session.handle() ;

    try {
        SQLite::Command cmd(con, "INSERT INTO metadata (name, value) VALUES (?, ?)") ;
        cmd.bind(1, name) ;
        cmd.bind(2, val) ;

        cmd.exec() ;
        return true ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
}


bool MBTileWriter::writeTilesDB(const MapFile &map, MapConfig &cfg)
{
    assert(db_) ;


    // write metadata
    writeMetaData("name", cfg.name_) ;
    writeMetaData("version", "1.1") ;
    writeMetaData("type", "baselayer") ;

    writeMetaData("description", cfg.description_) ;
    writeMetaData("attribution", cfg.attribution_) ;

    SQLite::Session session(db_.get()) ;
    SQLite::Connection &con = session.handle() ;

    try {

        SQLite::Transaction trans(con) ;
        SQLite::Command cmd(con, "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);") ;

        for( uint32_t z = cfg.minz_ ; z <= cfg.maxz_ ; z++ )
        {
            int32_t x0, y0, x1, y1 ;
            tms::metersToTile(cfg.bbox_.minx_, cfg.bbox_.miny_, z, x0, y0) ;
            tms::metersToTile(cfg.bbox_.maxx_, cfg.bbox_.maxy_, z, x1, y1) ;

//#pragma omp parallel for
            for( uint32_t x = x0 ; x<=x1 ; x++ )
                for(uint32_t y = y0 ; y<=y1 ; y++ )
                {

                    VectorTileWriter vt(x, y, z) ;

                    if ( map.queryTile(cfg, vt) ) {

                        string data = vt.toString() ;

                        cmd.bind((int)z) ;
                        cmd.bind((int)x) ;
                        cmd.bind((int)y) ;
                        cmd.bind(data.data(), data.size()) ;

                        cmd.exec() ;
                        cmd.clear() ;
                    }
                }
        }

        trans.commit() ;

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        cerr << e.what() << endl ;
        return false ;
    }
}

bool MBTileWriter::writeTilesFolder(const MapFile &map, MapConfig &cfg)
{
    for( uint32_t z = cfg.minz_ ; z <= cfg.maxz_ ; z++ )
    {
        int32_t x0, y0, x1, y1 ;
        tms::metersToTile(cfg.bbox_.minx_, cfg.bbox_.miny_, z, x0, y0) ;
        tms::metersToTile(cfg.bbox_.maxx_, cfg.bbox_.maxy_, z, x1, y1) ;

        for( uint32_t x = x0 ; x<=x1 ; x++ )
            for(uint32_t y = y0 ; y<=y1 ; y++ )
            {
                VectorTileWriter vt(x, y, z) ;

                if ( map.queryTile(cfg, vt) ) {

                    string data = vt.toString() ;

                    fs::path tile(tileset_) ;

                    tile /= to_string(z) ;
                    tile /= to_string(x) ;

                    fs::create_directories(tile) ;

                    tile /= to_string(y) + ".pbf";

                    ofstream strm(tile.native().c_str(), ios::binary) ;
                    strm.write(data.data(), data.size()) ;
                }
            }
    }

    return true ;

}

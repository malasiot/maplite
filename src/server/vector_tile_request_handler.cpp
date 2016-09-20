#include "vector_tile_request_handler.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "base64.hpp"
#include "logger.hpp"
#include "png_writer.hpp"
#include <boost/algorithm/string.hpp>

#include <fstream>

using namespace std ;
namespace fs = boost::filesystem ;
using namespace http ;

VectorTileRequestHandler::VectorTileRequestHandler(const string &id, const string &tileSet): TileRequestHandler(id, tileSet)
{
    if ( !fs::is_directory(tileset_) )
        db_.reset(new SQLite::Database(tileset_.native())) ;
}


void VectorTileRequestHandler::handle_request(const Request &request, Response &resp)
{

    boost::smatch m ;
    string sq = request.path_.substr(key_.length()) ;

    assert(boost::regex_match(sq, m, uri_pattern_)) ;

    int zoom = stoi(m.str(1)) ;
    int tx = stoi(m.str(2)) ;
    int ty = stoi(m.str(3)) ;
    string extension = m.str(4) ;

    ty = pow(2, zoom) - 1 - ty ;

    LOG_INFO_STREAM("Recieved request for tile: (" << tx << '/' << ty << '/' << zoom << ")" << " of key " << key_) ;

    string encoding, mime ;

    if ( extension == "pbf" ) {
        encoding = "gzip" ;
        mime = "application/x-protobuf" ;
    }
    else if ( extension == "png" )
        mime = "image/png" ;

    // since we do not store timestamps per tile we use the modification time of the tileset
    time_t mod_time = boost::filesystem::last_write_time(tileset_.native());

    if ( !fs::is_directory(tileset_) ) {

        SQLite::Session session(db_.get()) ;
        SQLite::Connection &con = session.handle() ;

        try {
            SQLite::Query q(con, "SELECT tile_data FROM tiles WHERE zoom_level=? AND tile_column=? AND tile_row=?") ;

            q.bind(zoom) ;
            q.bind(tx) ;
            q.bind(ty) ;

            SQLite::QueryResult res = q.exec() ;

            if ( res ) {
                int blobsize ;
                const char *data = res.getBlob(0, blobsize) ;

                string content ;

                content.assign(data, data + blobsize) ;

                resp.encode_file_data(content, encoding, mime, mod_time) ;
            }
            else {
                string empty_tile ;

                if ( extension == "pbf") // empty vector tile payload
                    empty_tile = base64_decode("H4sIAAAAAAAAAwMAAAAAAAAAAAA=") ;
                else if ( extension == "png" ) // empty png image payload
                    empty_tile = base64_decode(g_empty_transparent_png_256) ;

                if ( empty_tile.empty() )
                    resp = Response::stock_reply(Response::not_implemented) ;
                else
                    resp.encode_file_data(empty_tile, encoding, mime, mod_time) ;
            }

        }
        catch ( SQLite::Exception &e )
        {
            resp = Response::stock_reply(Response::internal_server_error) ;
            cerr << e.what() << endl ;
        }
    }
    else { // tiles are stored int the filesystem as seperate files

        // TODO fix to call renderer when extension is
        fs::path tile_path(tileset_) ;
        tile_path /= to_string(zoom) ;
        tile_path /= to_string(tx) ;
        tile_path /= to_string(ty) + '.' + extension;

        if ( fs::exists( tile_path ) )
            resp.encode_file(tile_path.native(), encoding, mime) ;
        else {
            string empty_tile ;

            if ( extension == "pbf") // empty vector tile payload
                empty_tile = base64_decode("H4sIAAAAAAAAAwMAAAAAAAAAAAA=") ;
            else if ( extension == "png" ) // empty png image payload
                empty_tile = base64_decode(g_empty_transparent_png_256) ;

            if ( empty_tile.empty() )
                resp = Response::stock_reply(Response::not_implemented) ;
            else
                resp.encode_file_data(empty_tile, encoding, mime, mod_time) ;
        }
    }
}

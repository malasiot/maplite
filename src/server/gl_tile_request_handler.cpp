#include "gl_tile_request_handler.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "base64.hpp"
#include "logger.hpp"

#include <fstream>

using namespace std ;
namespace fs = boost::filesystem ;
using namespace http ;

GLTileRequestHandler::GLTileRequestHandler(const string &id, const string &tileSet, std::shared_ptr<GLRenderingLoop> &renderer):
    TileRequestHandler(id, tileSet), gl_(renderer)
{
    if ( !fs::is_directory(tileset_) )
        db_.reset(new SQLite::Database(tileset_.native())) ;
}

extern const char *g_empty_transparent_png_256 ;

void GLTileRequestHandler::handle_request(const Request &request, Response &resp)
{
    boost::smatch m ;
    boost::regex_match(request.path_.substr(key_.length()), m, uri_pattern_) ;

    int zoom = stoi(m.str(1)) ;
    int tx = stoi(m.str(2)) ;
    int ty = stoi(m.str(3)) ;
    string extension = m.str(4) ;

    Dictionary options ;
    if ( request.method_ == "GET" )
        options = request.GET_ ;
    else if ( request.method_ == "POST" )
        options = request.POST_ ;

    ty = pow(2, zoom) - 1 - ty ;

    LOG_INFO_STREAM("Recieved request for tile: (" << tx << '/' << ty << '/' << zoom << ")" << "of map " << key_) ;

    string encoding, mime = "image/png" ;

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

                if ( gl_ ){
                    // we have to add a job to the OpenGL rendering loop and wait for it to become ready
                    content = gl_->addJob(tx, ty, zoom,
                                          std::string(data, blobsize), options)->get_future().get() ;
                }

                resp.encode_file_data(content, encoding, mime, mod_time) ;
            }
            else {
                string empty_tile = base64_decode(g_empty_transparent_png_256) ;

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
        tile_path /= to_string(ty) + ".pbf";

        if ( fs::exists(tile_path) && gl_ ) {
           ifstream strm(tile_path.native().c_str(), ios::binary) ;
           std::stringstream buffer;
           buffer << strm.rdbuf();
           strm.close();
           string content = gl_->addJob(tx, ty, zoom, buffer.str(), options)->get_future().get() ;
           resp.encode_file_data(content, encoding, mime, mod_time) ;
        }
        else {
           string empty_tile = base64_decode(g_empty_transparent_png_256) ;
           resp.encode_file_data(empty_tile, encoding, mime, mod_time) ;
        }
    }
}

#include "mapsforge_tile_request_handler.hpp"
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

bool MapsforgeTileRequestHandler::g_init_tile_index_ = false ;

MapsforgeTileRequestHandler::MapsforgeTileRequestHandler(const string &id, const string &map_file, const string &theme_file,
                                                         const std::string &layer,
                                                         const std::shared_ptr<FileSystemTileCache> &cache, bool debug):
    TileRequestHandler(id, map_file), layer_(layer), cache_(cache)
{
    if ( !g_init_tile_index_) {
        MapFileReader::initTileCache(1000000) ;
        g_init_tile_index_ = true ;
    }

    map_file_.reset(new MapFileReader()) ;

    try {
        map_file_->open(map_file) ;

    }
    catch ( std::runtime_error &e ) {
        LOG_FATAL_STREAM("Error opening map file: " << tileset_) ;
        return ;
    }

    theme_.reset(new RenderTheme()) ;

    if ( !theme_->read(theme_file) ) {
         LOG_FATAL_STREAM("Error reading map theme :" << theme_file) ;
         return ;
    }

    if ( layer.empty() ) layer_ = theme_->defaultLayer() ;

    renderer_.reset(new Renderer(theme_, map_file_->getMapFileInfo().lang_preference_, debug)) ;
}


void MapsforgeTileRequestHandler::handle_request(const Request &request, Response &resp)
{
    if ( !renderer_ ) {
        resp.status_ = Response::internal_server_error ;
        return ;
    }
    boost::smatch m ;
    string sq = request.path_.substr(key_.length()+2) ;

    int zoom, tx, ty ;

    if ( boost::regex_match(sq, m, TileRequestHandler::uri_pattern_)  ){
        zoom = stoi(m.str(1)) ;
        tx = stoi(m.str(2)) ;
        ty = stoi(m.str(3)) ;
    }
    else {
        resp.status_ = Response::bad_request ;
    }

    LOG_INFO_STREAM("Recieved request for tile: (" << tx << '/' << ty << '/' << zoom << ")" << " of key " << key_) ;

    // since we do not store timestamps per tile we use the modification time of the tileset
    time_t mod_time = boost::filesystem::last_write_time(tileset_.native());

    TileKey tk(tx, ty, zoom, true) ;

    string content = cache_->load(key_, tk, mod_time) ;
    if ( content.empty() ) { // not found in cache
        VectorTile tile = map_file_->readTile(tk);
        ImageBuffer buf(256, 256) ;
        renderer_->render(tk, buf, tile, layer_, 128) ;
        buf.saveToPNGBuffer(content);
        cache_->save(key_, tk, content, mod_time) ;
    }

    resp.encode_file_data(content, "", "image/png", mod_time) ;
}

#include "debug_tile_request_handler.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "logger.hpp"

#include <boost/algorithm/string.hpp>

#include <fstream>

using namespace std ;
namespace fs = boost::filesystem ;
using namespace http ;


DebugTileRequestHandler::DebugTileRequestHandler(const string &id):
    TileRequestHandler(id, string())
{

}


void DebugTileRequestHandler::handle_request(const Request &request, Response &resp)
{
    int tx, ty, zoom ;
    string sq = request.path_.substr(key_.length()) ;

    boost::smatch m ;
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
    time_t mod_time ;

    TileKey key(tx, ty, zoom, true) ;


    ImageBuffer buf(256, 256) ;

    renderer_.render(buf, key) ;

    string content ;
    buf.saveToPNGBuffer(content);
    resp.encode_file_data(content, "", "image/png", mod_time) ;

}

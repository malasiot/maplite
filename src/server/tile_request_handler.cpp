#include "tile_request_handler.hpp"

#include <boost/algorithm/string.hpp>

using namespace std ;
namespace fs = boost::filesystem ;

const boost::regex TileRequestHandler::uri_pattern_(R"((\d+)/(\d+)/(\d+)\.([^/]+))") ;

TileRequestHandler::TileRequestHandler(const string &id, const string &tileSet):
    tileset_(tileSet), key_(id)
{
}


bool TileRequestHandler::matches(const string &req_path) {
    if ( !boost::starts_with(req_path,  key_) ) return false ;
    if ( !boost::regex_match(req_path.substr(key_.length()), uri_pattern_) ) return false ;
    return true ;
}

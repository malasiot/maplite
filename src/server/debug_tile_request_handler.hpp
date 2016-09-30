#ifndef __DEBUG_TILE_REQUEST_HANDLER_HPP__
#define __DEBUG_TILE_REQUEST_HANDLER_HPP__

#include "tile_request_handler.hpp"
#include "renderer.hpp"


#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

class DebugTileRequestHandler: public TileRequestHandler {
public:

    DebugTileRequestHandler(const std::string &map_id) ;
    void handle_request(const http::Request &request, http::Response &resp) ;

private:

    DebugRenderer renderer_ ;
};

#endif

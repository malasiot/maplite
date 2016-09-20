#ifndef __RASTER_REQUEST_HANDLER_HPP__
#define __RASTER_REQUEST_HANDLER_HPP__

#include "tile_request_handler.hpp"
#include "jp2_decoder.hpp"
#include "raster_tile_cache.hpp"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

class RasterRequestHandler: public TileRequestHandler {
public:

    RasterRequestHandler(const std::string &map_id, const std::string &tileSet) ;
    void handle_request(const http::Request &request, http::Response &resp) ;

public:
    JP2Decoder provider_ ;
    static RasterTileCache tile_cache_ ;
};

#endif

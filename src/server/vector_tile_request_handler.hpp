#ifndef __VECTOR_TILE_REQUEST_HANDLER_HPP__
#define __VECTOR_TILE_REQUEST_HANDLER_HPP__

#include "tile_request_handler.hpp"
#include "database.hpp"


#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

class VectorTileRequestHandler: public TileRequestHandler {
public:

    VectorTileRequestHandler(const std::string &map_id, const std::string &tileSet) ;
    void handle_request(const http::Request &request, http::Response &resp) ;

private:

    std::unique_ptr<SQLite::Database> db_ ;
};

#endif

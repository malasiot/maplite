#ifndef __GL_TILE_REQUEST_HANDLER_HPP__
#define __GL_TILE_REQUEST_HANDLER_HPP__

#include "tile_request_handler.hpp"
#include "database.hpp"
#include "gl_rendering_loop.hpp"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

class GLTileRequestHandler: public TileRequestHandler {
public:

    GLTileRequestHandler(const std::string &map_id, const std::string &tileSet, std::shared_ptr<GLRenderingLoop> &renderer) ;
    void handle_request(const http::Request &request, http::Response &resp) ;

private:

    friend class GLRenderingLoop ;
    std::unique_ptr<SQLite::Database> db_ ;
    std::shared_ptr<GLRenderingLoop> gl_ ;
};

#endif

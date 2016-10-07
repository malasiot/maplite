#ifndef __MAPSFORGE_TILE_REQUEST_HANDLER_HPP__
#define __MAPSFORGE_TILE_REQUEST_HANDLER_HPP__

#include "tile_request_handler.hpp"
#include "database.hpp"
#include "mapsforge_database.hpp"
#include "theme.hpp"
#include "renderer.hpp"


#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

class MapsforgeTileRequestHandler: public TileRequestHandler {
public:

    MapsforgeTileRequestHandler(const std::string &map_id, const std::string &map_file_name, const std::string &theme_file_name, bool debug) ;
    void handle_request(const http::Request &request, http::Response &resp) ;

private:

    static std::shared_ptr<TileIndex> tile_index_ ;
    std::shared_ptr<MapFile> map_file_ ;

    RenderTheme theme_ ;
    std::shared_ptr<Renderer> renderer_ ;
};

#endif

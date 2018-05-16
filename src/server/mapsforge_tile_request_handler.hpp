#ifndef __MAPSFORGE_TILE_REQUEST_HANDLER_HPP__
#define __MAPSFORGE_TILE_REQUEST_HANDLER_HPP__

#include "tile_request_handler.hpp"
#include "database.hpp"
#include "mapsforge_map_reader.hpp"
#include "theme.hpp"
#include "renderer.hpp"
#include "fs_tile_cache.hpp"


#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

class MapsforgeTileRequestHandler: public TileRequestHandler {
public:

    MapsforgeTileRequestHandler(const std::string &map_id,
                                const std::string &map_file_name,
                                const std::string &theme_file_name,
                                const std::string &layer,
                                const std::string &language,
                                const std::shared_ptr<FileSystemTileCache> &cache,
                                bool debug) ;
    void handle_request(const http::Request &request, http::Response &resp) ;

private:

    static bool g_init_tile_index_ ;
    std::shared_ptr<MapFileReader> map_file_ ;
    std::shared_ptr<RenderTheme> theme_ ;
    std::shared_ptr<Renderer> renderer_ ;
    std::string layer_, lang_ ;
    std::shared_ptr<FileSystemTileCache> cache_ ;
};

#endif

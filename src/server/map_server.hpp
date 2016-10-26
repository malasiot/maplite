#ifndef __MAP_SERVER_H__
#define __MAP_SERVER_H__

#include "server.hpp"
#include "fs_tile_cache.hpp"

class MapServer: public http::Server {
public:
    MapServer(const std::string &config_file, const std::string &host_name, const std::string &ports, const std::string &cache_dir = std::string()) ;
};

#endif

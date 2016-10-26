#include "map_server.hpp"
#include "map_server_handler_factory.hpp"

using namespace std ;

MapServer::MapServer(const string &config_file, const string &host, const string &ports, const std::string &cache_dir):
    http::Server(std::make_shared<MapServerHandlerFactory>(config_file, make_shared<FileSystemTileCache>(cache_dir)), host, ports, 4) {
}

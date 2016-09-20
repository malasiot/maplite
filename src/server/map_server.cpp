#include "map_server.hpp"
#include "map_server_handler_factory.hpp"

using namespace std ;

MapServer::MapServer(const string &root_folders, const string &ports, bool enable_gl):
    http::Server(std::make_shared<MapServerHandlerFactory>(root_folders, enable_gl), "127.0.0.1", ports, 4) {
}

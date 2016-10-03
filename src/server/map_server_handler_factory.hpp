#ifndef __MAP_SERVER_HANDLER_FACTORY_HPP__
#define __MAP_SERVER_HANDLER_FACTORY_HPP__

#include "request_handler_factory.hpp"
#include "request_handler.hpp"
#include "mapsforge_tile_request_handler.hpp"
#include "asset_request_handler.hpp"

#include <map>


class MapServerHandlerFactory: public http::RequestHandlerFactory {
public:

    MapServerHandlerFactory(const std::string &root_folders)  ;
    std::shared_ptr<http::RequestHandler> create(const http::Request &req) ;

private:

    std::map<std::string, std::shared_ptr<http::RequestHandler> > tile_request_handlers_ ;
    std::map<std::string, std::shared_ptr<AssetRequestHandler> > asset_request_handlers_ ;
};

#endif

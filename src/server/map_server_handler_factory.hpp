#ifndef __MAP_SERVER_HANDLER_FACTORY_HPP__
#define __MAP_SERVER_HANDLER_FACTORY_HPP__

#include "request_handler_factory.hpp"
#include "request_handler.hpp"
#include "gl_rendering_loop.hpp"
#include "vector_tile_request_handler.hpp"
#include "asset_request_handler.hpp"

#include <map>


class MapServerHandlerFactory: public http::RequestHandlerFactory {
public:

    MapServerHandlerFactory(const std::string &root_folders, bool enable_gl=false)  ;
    ~MapServerHandlerFactory() {
//        shutdown_gl_loop() ;
    }
    std::shared_ptr<http::RequestHandler> create(const http::Request &req) ;

private:
/*
    void shutdown_gl_loop() {
        if ( gl_ ) {
            gl_->stop() ;
            gl_thread_->join() ;
        }
    }
*/
    std::map<std::string, std::shared_ptr<http::RequestHandler> > tile_request_handlers_ ;
    std::map<std::string, std::shared_ptr<AssetRequestHandler> > asset_request_handlers_ ;
 //   std::shared_ptr<GLRenderingLoop> gl_ ;
//    std::unique_ptr<std::thread> gl_thread_ ;
};

#endif

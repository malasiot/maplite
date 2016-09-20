#ifndef __TILE_REQUEST_HANDLER_HPP__
#define __TILE_REQUEST_HANDLER_HPP__

#include "request_handler.hpp"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

class TileRequestHandler: public http::RequestHandler {
public:

    TileRequestHandler(const std::string &key, const std::string &tileset) ;
    virtual void handle_request(const http::Request &request, http::Response &resp) = 0;
    virtual bool matches(const std::string &req_path) ;

protected:

    boost::filesystem::path tileset_ ;
    std::string key_ ;

public:
    static const boost::regex uri_pattern_ ;
};

#endif

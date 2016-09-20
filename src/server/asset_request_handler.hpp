#ifndef __ASSET_REQUEST_HANDLER_HPP__
#define __ASSET_REQUEST_HANDLER_HPP__

#include "request_handler.hpp"
#include "database.hpp"

#include <boost/filesystem.hpp>

class AssetRequestHandler: public http::RequestHandler {
public:

    AssetRequestHandler(const std::string &url_prefix, const std::string &rsdb) ;
    void handle_request(const http::Request &request, http::Response &resp) ;
    bool matches(const std::string &req_path) ;

private:

    std::unique_ptr<SQLite::Database> db_ ;
    boost::filesystem::path rsdb_ ;
    std::string url_prefix_ ;
};

#endif

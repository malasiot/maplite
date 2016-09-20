#ifndef HTTP_SERVER_REQUEST_HANDLER_FACTORY
#define HTTP_SERVER_REQUEST_HANDLER_FACTORY

#include "request_handler.hpp"

// modeled after POCO Net library

namespace http {
class RequestHandlerFactory {
public:
    RequestHandlerFactory() = default ;

    // implement this to return a handler matching the request

    virtual std::shared_ptr<RequestHandler> create(const Request &req) = 0;
};


}

#endif

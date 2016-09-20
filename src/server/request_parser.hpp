//
// request_parser.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_REQUEST_PARSER_HPP
#define HTTP_SERVER_REQUEST_PARSER_HPP

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include <map>

#include "http_parser.h"

namespace http {

struct Request;

namespace detail {

/// Parser for incoming requests.
class RequestParser
{
public:
    /// Construct ready to parse the request method.
    RequestParser();

    /// Reset to initial parser state.
    void reset();

    /// call the parser with chunk of data while it is in indeterminate state
    boost::tribool parse(const char *data, size_t buf_len) ;

    // fill in the request structure
    bool decode_message(Request &req) const ;

private:

    static int on_message_begin(http_parser * parser);
    static int on_message_complete(http_parser *parser);
    static int on_header_field(http_parser *parser, const char *data, size_t size);
    static int on_header_value(http_parser *parser, const char *data, size_t size);
    static int on_headers_complete (http_parser * parser);
    static int on_url(http_parser * parser, const char *data, size_t size);
    static int on_body(http_parser * parser, const char *data, size_t size) ;

protected:
    http_parser parser_ ;
    http_parser_settings settings_ ;

    std::string current_header_field_, current_header_value_, url_, body_ ;
    std::map<std::string, std::string> headers_ ;
    bool is_complete_ ;
};

} // namespace detail
} // namespace http

#endif // HTTP_SERVER2_REQUEST_PARSER_HPP

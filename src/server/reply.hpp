//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_REPLY_HPP
#define HTTP_SERVER_REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>

#include "dictionary.hpp"

namespace http {

/// A reply to be sent to a client.
struct Response
{
    /// The status of the reply.
    enum status_type
    {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    } status_;

    /// The headers to be included in the reply.
    Dictionary headers_;

    /// The content to be sent in the reply.
    std::string content_;


    /// Get a stock reply.
    static Response stock_reply(status_type status);

    // this will fill in the reply for sending over a file payload

    void encode_file_data(const std::string &bytes,
                     const std::string &encoding,
                     const std::string &mime,
                     const time_t mod_time) ;

    void encode_file(const std::string &path_name,
                     const std::string &encoding,
                     const std::string &mime) ;
};


} // namespace http

#endif // HTTP_SERVER2_REPLY_HPP

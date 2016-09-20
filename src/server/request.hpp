//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_REQUEST_HPP
#define HTTP_SERVER_REQUEST_HPP

#include <string>
#include <vector>

#include "dictionary.hpp"

namespace http {

/// A request received from a client.
struct Request
{

    Dictionary SERVER_ ; // Server variables
    Dictionary GET_ ;	 // Query variables for GET requests
    Dictionary POST_ ;   // Post variables for POST requests
    Dictionary COOKIE_ ; // Cookies

    struct UploadedFile {
        std::string orig_name_ ;	// The original filename
        std::string server_path_ ; // The path of a local temporary copy of the uploaded file
        std::string mime_ ;		// MIME information of the uploaded file
    } ;

    std::map<std::string, UploadedFile> FILE_ ;	// Uploaded files

    // This is content sent using POST with Content-Type other than
    // x-www-form-urlencoded or multipart-form-data e.g. text/xml

    std::string content_ ;
    std::string content_type_ ;

    std::string method_;
    std::string path_;
    std::string query_ ;

    int http_version_major_;
    int http_version_minor_;

private:
    friend class RequestParser ;
    void parse_headers(const std::map<std::string, std::string> &headers) ;
};


} // namespace http

#endif // HTTP_SERVER2_REQUEST_HPP

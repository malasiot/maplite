//
// RequestParser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// adopted from C++ wrapper of nodejs parser
// https://github.com/AndreLouisCaron/httpxx

#include "request_parser.hpp"
#include "request.hpp"

#include <algorithm>
#include <cstring>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#include <boost/iostreams/stream.hpp>

using namespace std ;

namespace http {
namespace detail {

RequestParser::RequestParser()
{
    memset(&settings_, 0, sizeof(settings_));
    settings_.on_url = &on_url;

    settings_.on_message_complete = &on_message_complete;
    settings_.on_message_begin    = &on_message_begin;
    settings_.on_header_field     = &on_header_field;
    settings_.on_header_value     = &on_header_value;
    settings_.on_headers_complete = &on_headers_complete;
    settings_.on_body = &on_body;

    reset() ;
}

void RequestParser::reset()
{
    memset(&parser_, 0, sizeof(parser_));
    http_parser_init(&parser_, HTTP_REQUEST);

    parser_.data = this ;
    url_.clear() ;
    current_header_field_.clear() ;
    current_header_value_.clear() ;
    body_.clear() ;
}


int RequestParser::on_message_begin(http_parser *parser)
{
    RequestParser &rp = *static_cast<RequestParser*>(parser->data);
    rp.is_complete_ = false ;
    return 0;
}

int RequestParser::on_message_complete(http_parser * parser)
{
    RequestParser &rp = *static_cast<RequestParser*>(parser->data);
    rp.is_complete_ = true ;

    // Force the parser to stop after the Request is parsed so clients
    // can process the Request (or response).  This is to properly
    // handle HTTP/1.1 pipelined Requests.
    http_parser_pause(parser, 1);

    return 0;
}

int RequestParser::on_header_field(http_parser *parser, const char *data, size_t size)
{
    RequestParser &rp = *static_cast<RequestParser*>(parser->data);

    if ( !rp.current_header_value_.empty() ) {
        rp.headers_[rp.current_header_field_] = rp.current_header_value_ ;
        rp.current_header_field_.clear() ;
        rp.current_header_value_.clear() ;
    }
    rp.current_header_field_.append(data, size);
    return 0 ;
}

int RequestParser::on_header_value(http_parser *parser, const char *data, size_t size)
{
    RequestParser& rp = *static_cast<RequestParser*>(parser->data);
    rp.current_header_value_.append(data, size);
    return 0 ;
}

int RequestParser::on_headers_complete(http_parser * parser)
{
    RequestParser &rp = *static_cast<RequestParser*>(parser->data);

    if ( !rp.current_header_value_.empty() ) {
        rp.headers_[rp.current_header_field_] = rp.current_header_value_ ;
        rp.current_header_field_.clear() ;
        rp.current_header_value_.clear() ;
    }

    // Force the parser to stop after the headers are parsed so clients
    // can process the Request (or response).  This is to properly
    // handle HTTP/1.1 pipelined Requests.
    http_parser_pause(parser, 1);

    return 0;
}

int RequestParser::on_url(http_parser *parser, const char *data, size_t size)
{
    RequestParser& rp = *static_cast<RequestParser*>(parser->data);
    rp.url_.append(data, size);

    return 0;
}

int RequestParser::on_body(http_parser * parser, const char *data, size_t size)
{
    static_cast<RequestParser*>(parser->data)->body_.append(data, size) ;
    return 0;
}



boost::tribool RequestParser::parse(const char *data, size_t size)
{
    std::size_t used = http_parser_execute(&parser_, &settings_, data, size);

    const http_errno error = static_cast< http_errno >(parser_.http_errno);

  // The 'on_message_complete' and 'on_headers_complete' callbacks fail
  // on purpose to force the parser to stop between pipelined Requests.
  // This allows the clients to reliably detect the end of headers and
  // the end of the message.  Make sure the parser is always unpaused
  // for the next call to 'feed'.

  if (error == HPE_PAUSED) {
      http_parser_pause(&parser_, 0);
  }

  if (used < size)
  {
      if (error == HPE_PAUSED)
      {
          // Make sure the byte that triggered the pause
          // after the headers is properly processed.
          if ( !is_complete_ )
              used += http_parser_execute(&parser_, &settings_, data + used, size - used);
      }
      else {
          return false ;
      }
  }

  return ( is_complete_ ? boost::tribool(true) : boost::indeterminate ) ;
}

/////////////////////////////////////////////////////////////////////////////

static int hex_decode(char c)
{
    char ch = tolower(c) ;

    if ( ch >= 'a' && ch <= 'f' ) return 10 + ch - 'a' ;
    else if ( ch >= '0' && ch <= '9' ) return ch - '0' ;
    else return 0 ;
}

std::string url_decode(const char *str)
{
    const char *p = str ;

    std::string ret ;
    while ( *p )
    {
        if( *p == '+' ) ret += ' ' ;
        else if ( *p == '%' )
        {
            ++p ;
            char tmp[4];
            unsigned char val = 16 * ( hex_decode(*p++) )  ;
            val += hex_decode(*p) ;
            sprintf(tmp,"%c", val);
            ret += tmp ;
        } else ret += *p ;
        ++p ;
    }

    return ret;
}

static bool has_url_field( http_parser_url &url, http_parser_url_fields field )
{
    return ((url.field_set & (1 << int(field))) != 0);
}

static std::string get_url_field ( const string &data, http_parser_url &url, http_parser_url_fields field ) {
    if ( !has_url_field(url, field) ) return string() ;
    return ( data.substr(url.field_data[int(field)].off, url.field_data[int(field)].len) );
}

static bool parse_url(Request &req, const string url)
{
    http_parser_url u ;

    int result = http_parser_parse_url(url.c_str(), url.length(), 0, &u);

    if ( result ) return false ;

    string uri = get_url_field(url, u, UF_PATH) ;
    string query = get_url_field(url, u, UF_QUERY) ;

    req.path_ = url_decode(uri.c_str()) ;
    req.query_ = url_decode(query.c_str()) ;

    if ( !req.query_.empty() )
    {
        std::vector<std::string> args ;

        boost::split(args, req.query_, boost::is_any_of("&"), boost::algorithm::token_compress_on);

        for(int i=0 ; i<args.size() ; i++ )
        {
            std::string &arg = args[i] ;

            int pos = arg.find('=') ;

            if ( pos > 0 )
            {
                std::string key = arg.substr((int)0, (int)pos) ;
                std::string val = arg.substr((int)pos+1, -1) ;

                req.GET_[key] = ( val.empty() ) ? "" : val ;
            }
            else if ( pos == -1 )
                req.GET_[arg] = "" ;
        }
    }

    return true ;

}

static bool parse_cookie(Request &session, const std::string &data)
{
    int pos = data.find("=") ;

    if ( pos == -1 ) return false ;

    boost::regex rx("[ \n\r\t\f\v]*") ;
    boost::smatch rm ;

    boost::regex_search(data, rm, rx) ;
    int wscount = rm[0].length() ;

    std::string name = data.substr(wscount, pos - wscount);
    std::string value = data.substr(++pos);

    session.COOKIE_[name] = value ;

    return true ;
}

static bool parse_cookies(Request &session)
{
    const char *ck = "Cookie" ;

    if ( session.SERVER_.count(ck) == 0 ) return true ;

    std::string data = session.SERVER_[ck] ;

    if ( data.empty() ) return false ;

    int  old_pos = 0 ;

    while (1)
    {
        // find the ';' terminating a name=value pair
        int pos = data.find(";", old_pos);

        // if no ';' was found, the rest of the string is a single cookie

        if ( pos == -1 ) {
            bool res = parse_cookie(session, data.substr(old_pos, (int)-1));
            return res ;
        }

        // otherwise, the string contains multiple cookies
        // extract it and add the cookie to the list
        if ( !parse_cookie(session, data.substr(old_pos, pos - old_pos)) ) return false ;

        // update pos (+1 to skip ';')
        old_pos = pos + 1;
    }

    return true ;
}

static std::string get_next_line(std::istream &strm, int maxc = 1000)
{
    std::string res ;
    char b0, b1 ;
    int count = 0 ;

    while ( count < maxc && !strm.eof() )
    {
        b0 = strm.get() ;
        count ++ ;

        if ( b0 == '\r' )
        {
            b1 = strm.get() ;
            count ++ ;

            if ( b1 == '\n' ) return res ;
            else {
                res += b0 ;
                res += b1 ;
            }
        }
        else res += b0 ;

    }

    return res ;
}

namespace fs = boost::filesystem ;

fs::path get_temporary_path(const std::string &dir, const std::string &prefix, const std::string &ext)
{
    std::string retVal ;

    fs::path directory ;

    if ( ! dir.empty() ) directory = dir;
    else directory = boost::filesystem::temp_directory_path() ;

    std::string varname ="%%%%-%%%%-%%%%-%%%%";

    if ( !prefix.empty() )
        directory /= prefix + '-' + varname + '.' + ext ;
    else
        directory /= "tmp-" + varname + '.' + ext ;

    boost::filesystem::path temp = boost::filesystem::unique_path(directory);

    return temp;
}


static bool parse_mime_data(Request &session, istream &strm, const char *fld, const char *file_name,
                          const char *content_type,
                          const char *trans_encoding,
                          const char *bnd)
{
    std::string data ;

    while ( 1 )
    {
        char b0 = strm.get() ;

        if ( b0 == '\r' )
        {
            char b1 = strm.get() ;

            if ( b1 == '\n' )
            {
                char b2 = strm.get() ;

                if ( b2 == '-' )
                {
                    char b3 = strm.get() ;

                    if ( b3 == '-' )
                    {
                        int bndlen = strlen(bnd) ;
                        char *buf = new char [bndlen] ;
                        strm.read(buf, bndlen) ;

                        if ( strncmp(buf, bnd, bndlen ) == 0 ) {
                            strm.get() ; strm.get() ;

                            delete buf ;
                            break ;
                        }
                        delete buf ;
                    }
                    else
                    {
                        data += b0 ;
                        data += b1 ;
                        data += b2 ;
                        data += b3 ;
                    }
                }
                else {
                    data += b0 ;
                    data += b1 ;
                    data += b2 ;
                }
            }
            else
            {
                data += b0 ;
                data += b1 ;
            }
        }
        else data += b0 ;
    }

    if ( file_name == 0 ) session.POST_[fld] = data ;
    else
    {
        Request::UploadedFile fileInfo ;

        fileInfo.mime_ = content_type ;
        fileInfo.orig_name_ = file_name ;

        boost::filesystem::path server_path = get_temporary_path(string(), "up", "tmp") ;

        FILE *file = fopen(server_path.string().c_str(), "wb") ;
        fwrite(data.data(), data.size(), 1, file) ;
        fclose(file) ;

        fileInfo.server_path_ = server_path.string() ;
        session.FILE_[fld] = fileInfo ;
    }

    return true ;
}


static bool parse_multipart_data(Request &session, istream &strm, const char *bnd)
{
    std::string s = get_next_line(strm) ;
    if ( s.empty() ) return false ;

    // Parse boundary header
    const char *p = s.c_str() ;
    if ( *p++ != '-' || *p++ != '-' || strncmp(p, bnd, s.length() ) != 0 ) return  false ;

    while ( 1 )
    {

        std::string form_field, file_name, content_type, trans_encoding;

        while ( 1 )
        {
            s = get_next_line(strm) ;
            if ( s.empty() ) break ;

            const char *p = s.c_str() ;
            const char *q = strchr(p, ':') ;

            if ( q )
            {
                std::string key, val ;
                key.assign(p, (int)(q - p)) ;
                boost::trim(key) ;
                val.assign(q+1) ;
                boost::trim(val) ;

                if ( strncmp(key.c_str(), "Content-Disposition", 20) == 0 )
                {
                    if ( strncmp(val.c_str(), "form-data", 9) == 0 )
                    {
                        const char *a = strchr((const char *)val.c_str() + 9, ';') ;
                        while ( a && *a )
                        {
                            ++a ;
                            std::string key_, val_ ;

                            while ( *a && *a != '=' ) key_ += *a++ ;
                            if ( *a == 0 ) return false ; ++a ;
                            while ( *a && *a != ';' ) val_ += *a++ ;
                            boost::trim(key_);
                            boost::trim_if(val_, boost::is_any_of(" \"")) ;

                            if ( key_ == "name" ) form_field = val_ ;
                            else if ( key_ == "filename" ) file_name = val_ ;


                        }
                    }
                }
                else if ( strncmp(key.c_str(), "Content-Type", 11) == 0 )
                    content_type = val ;
                else if ( strncmp(key.c_str(), "Content-Transfer-Encoding", 25) == 0 )
                    trans_encoding = val ;

            }
        }

        if ( form_field.empty() ) break;

        // Parse content

        if ( ! parse_mime_data(session, strm, form_field.c_str(),
                             ((file_name.empty()) ? (const char *)NULL : file_name.c_str()),
                             content_type.c_str(), trans_encoding.c_str(), bnd) ) return false ;


    }

    return true ;
}


static bool parse_form_data(Request &session, istream &strm)
{
    const string cl = "Content-Length" ;

    unsigned int content_length = 1000 ;
    bool has_content_length = false ;

    if ( session.SERVER_.count(cl) == 1 )
    {
        content_length = atoi(session.SERVER_[cl].c_str()) ;
        has_content_length = true ;
    }

    const char *ct = "Content-Type" ;

    if ( session.SERVER_.count(ct) == 0 ) return false ;

    std::string content_type = session.SERVER_[ct] ;

    if ( strncmp(content_type.c_str(), "application/x-www-form-urlencoded", 33) == 0 )
    {
        // parse name value pairs

        std::string s = get_next_line(strm, content_length) ;

        typedef boost::tokenizer<boost::char_separator<char>>   tokenizer;

        boost::char_separator<char> sep("&");
        tokenizer tokens(s, sep);

        for (tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it)
        {
            std::string str = (*it) ;

            int pos = str.find('=') ;
            if ( pos < 0 ) return false ;

            std::string key, val ;
            key = str.substr(0, pos) ;
            val = str.substr(pos+1) ;
            session.POST_[url_decode(key.c_str())] = url_decode(val.c_str()) ;
        }
    }
    else if ( strncmp(content_type.c_str(),"multipart/form-data", 19) == 0 )
    {
        std::string boundary ;

        const char *p = strstr((const char *)content_type.c_str() + 19, "boundary") ;
        if ( !p ) return false ;
        ++p ;
        p = strchr(p, '=') ;
        if ( !p ) return false ;
        ++p ;
        while ( *p == ' ' || *p == '"' ) ++p ;
        while ( *p != 0 && *p != '"' && *p != '\r' && *p != ';' && *p != ' ')
            boundary += *p++ ;

        return parse_multipart_data(session, strm, boundary.c_str()) ;
    }
    else if ( has_content_length )
    {

        session.content_.resize(content_length) ;
        strm.read(&session.content_[0], content_length) ;
        session.content_type_ = content_type ;

    }


    return true ;
}


bool RequestParser::decode_message(Request &req) const {

    for( auto hdr: headers_ )
        req.SERVER_.add(hdr.first, hdr.second) ;

    req.method_ = req.SERVER_["REQUEST_METHOD"] =	http_method_str(static_cast<http_method>(parser_.method)) ;
    req.http_version_major_ = parser_.http_major ;
    req.http_version_minor_ = parser_.http_minor ;
    req.content_ = body_ ;

    if ( !parse_url(req, url_) ||
        !parse_cookies(req) ) return false ;

    if ( req.method_ == "POST" )
    {
        stringstream reqstr(body_) ;

        if ( !parse_form_data(req, reqstr) ) return false ;
    }

    return true ;
}

} // namespace server
} // namespace http

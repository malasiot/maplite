//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "server.hpp"

using namespace http::server ;
using namespace std ;

class TileHandler: public request_handler {
public:
    void handle_request(const request& req, reply& rep) {

        boost::smatch m ;
        boost::regex_match(req.path_, m, rx_) ;

        int zoom = stoi(m.str(1)) ;
        int tx = stoi(m.str(2)) ;
        int ty = stoi(m.str(3)) ;
        string extension = m.str(4) ;

        cout << tx << ' ' << ty << ' ' << zoom << endl ;
    }

     static boost::regex rx_ ;
};

boost::regex TileHandler::rx_(R"(/map/[^/]+/tiles/[^/]+/(\d+)/(\d+)/(\d+)\.([^/]+))") ;

class Factory: public request_handler_factory {
public:
    Factory() = default ;

    std::shared_ptr<request_handler> create(const request &req) {
        boost::smatch m ;
        if ( boost::regex_match(req.path_, m, TileHandler::rx_) )
             return std::shared_ptr<request_handler>(new TileHandler()) ;
        else
            return nullptr ;
    }
};

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 5)
    {
      std::cerr << "Usage: http_server <address> <port> <threads> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 1 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 1 .\n";
      return 1;
    }

    // Initialise the server.
    std::size_t num_threads = boost::lexical_cast<std::size_t>(argv[3]);
    http::server::server s(make_shared<Factory>(), argv[1], argv[2], num_threads);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}

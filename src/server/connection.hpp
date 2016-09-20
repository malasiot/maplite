//
// HttpConnection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_CONNECTION_HPP
#define HTTP_SERVER_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_handler_factory.hpp"
#include "request_parser.hpp"


namespace http {
class ConnectionManager ;

/// Represents a single HttpConnection from a client.
class Connection
        : public std::enable_shared_from_this<Connection>
{
public:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    /// Construct a HttpConnection with the given io_service.
    explicit Connection(boost::asio::ip::tcp::socket socket,
                        ConnectionManager& manager, const std::shared_ptr<RequestHandlerFactory>& handler);

    /// Get the socket associated with the HttpConnection.
    boost::asio::ip::tcp::socket &socket() ;

    /// Start the first asynchronous operation for the HttpConnection.
    void start();

    void stop();

private:
    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code& e,
                     std::size_t bytes_transferred);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e);

    /// Socket for the HttpConnection.
    boost::asio::ip::tcp::socket socket_;

    /// The dispatcher of incoming HttpRequest.
    std::shared_ptr<RequestHandlerFactory> handler_factory_;

    /// Buffer for incoming data.
    boost::array<char, 8192> buffer_;

    ConnectionManager& connection_manager_ ;

    /// The incoming HttpRequest.
    Request request_;

    /// The parser for the incoming HttpRequest.
    detail::RequestParser request_parser_;

    /// The reply to be sent back to the client.
    Response reply_;
};

typedef std::shared_ptr<Connection> ConnectionPtr;

}
#endif // HTTP_SERVER_CONNECTION_HPP

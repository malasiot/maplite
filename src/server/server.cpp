//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"


namespace http {

Server::Server(const std::shared_ptr<RequestHandlerFactory> &handlers, const std::string& address, const std::string& port,
               std::size_t io_service_pool_size)
    : io_service_pool_(io_service_pool_size),
      signals_(io_service_pool_.get_io_service()),
      acceptor_(io_service_pool_.get_io_service()),
      socket_(io_service_pool_.get_io_service()),
      handler_factory_(handlers)

{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

    do_await_stop();

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_context());
    boost::asio::ip::tcp::resolver::query query(address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();
}

void Server::run()
{
    io_service_pool_.run();
}

void Server::start_accept()
{
    //new_connection_.reset(new connection(io_service_pool_.get_io_service(), handler_factory_));
    acceptor_.async_accept(socket_, [this] ( const boost::system::error_code& e ){

        // Check whether the server was stopped by a signal before this
             // completion handler had a chance to run.
             if (!acceptor_.is_open())
             {
               return;
             }

             if (!e)
             {
               connection_manager_.start(std::make_shared<Connection>(
                   std::move(socket_), connection_manager_, handler_factory_));
             }

        //if (!e) new_connection_->start();
        start_accept();
    }) ;
}

void Server::handle_stop()
{
    acceptor_.close();
    connection_manager_.stop_all();
    io_service_pool_.stop();
}


void Server::do_await_stop()
{
  signals_.async_wait(
      [this](boost::system::error_code /*ec*/, int /*signo*/)
      {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
            handle_stop() ;
      });
}

void Server::stop()
{
    handle_stop() ;
}

} // namespace http

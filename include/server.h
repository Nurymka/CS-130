#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#ifndef SERVER_H
#define SERVER_H

class server {
  public:
    server(boost::asio::io_service& io_service, short port)
      : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port));
  private:
    void start_accept();
    void handle_accept(session* new_session,
      const boost::system::error_code& error);
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
}

#endif

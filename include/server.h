#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "handler_manager.h"

#ifndef SERVER_H_
#define SERVER_H_

using boost::asio::ip::tcp;

class server {
 public:
    server(boost::asio::io_service& io_service,
          unsigned short port, HandlerManager* handlerManager);
    ~server();
 private:
    void start_accept();
    void handle_accept(session* new_session,
      const boost::system::error_code& error);
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    HandlerManager* handlerManager_;
};

#endif  // SERVER_H_

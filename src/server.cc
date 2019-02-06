//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "server.h"
#include "handler_manager.h"

// refactored server class from server_main.cc

server::server(boost::asio::io_service& io_service, unsigned short port,
  HandlerManager* handlerManager)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    handlerManager_(handlerManager) {
  start_accept();
}

server::~server() {
  delete handlerManager_;
}

void server::start_accept() {
  session* new_session = new session(io_service_, handlerManager_);
  acceptor_.async_accept(new_session->socket(),
    boost::bind(&server::handle_accept, this, new_session,
      boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
  const boost::system::error_code& error) {
  if (!error) {
    new_session->start();
  } else {
    delete new_session;
  }
  start_accept();
}

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
#include "logger.h"
#include "handler_manager.h"

// refactored server class from server_main.cc

server::server(boost::asio::io_service& io_service,
  int16_t port,
  const string& rootPath,
  LocationMap locationInfos,
  HandlerManager* handlerManager)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    rootPath_(rootPath),
    port_(port),
    locationInfos_(locationInfos),
    handlerManager_(handlerManager) {
  BOOST_LOG_SEV(Logger::get(), INFO) << "Server starting at port: " << port;
  start_accept();
}

server::~server() {
  // TODO(nurymka): will get removed once locationInfos_ has unique_ptr<LocationInfo>
  for (auto& locInfo : locationInfos_)
    delete locInfo.second;
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
    BOOST_LOG_SEV(Logger::get(), ERROR)
      << "Failed to establish new connection: "
      << error.message();
    delete new_session;
  }
  start_accept();
}

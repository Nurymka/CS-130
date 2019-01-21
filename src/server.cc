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

// refactored server class from server_main.cc

server::server(boost::asio::io_service& io_service, short port)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{
  start_accept();
  //std::cout<<"server"<< std::endl;
}

void server::start_accept()
{
  //std::cout<<"start_accept "<< std::endl;
  session* new_session = new session(io_service_);
  acceptor_.async_accept(new_session->socket(),
    boost::bind(&server::handle_accept, this, new_session,
      boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
  const boost::system::error_code& error)
{
  if (!error)
  {
    //std::cout<<"handle_accept "<< std::endl;
    new_session->start();
  }
  else
  {
    //std::cout<<"handle_accept error"<< std::endl;
    delete new_session;
  }
  start_accept();
}

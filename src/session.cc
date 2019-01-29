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
#include "http_request.h"
#include "http_response.h"

// refactored session class from server_main.cc
// part of the codes adapted or inspired from following urls:
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_parser.cpp
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp

#pragma region Static Methods

void session::handleGoodRequest(HttpRequest& httpRequest, HttpResponse& httpResponse) {
  httpResponse.version = httpRequest.version;
  httpResponse.status_code = 200;
  httpResponse.headers.push_back("Content-Type: text/plain");
  httpResponse.body = httpRequest.to_string();
}

void session::handleBadRequest(HttpRequest& httpRequest, HttpResponse& httpResponse) {
  httpResponse.version = "HTTP/1.1";
  httpResponse.status_code = 400;
  httpResponse.headers.push_back("Content-Type: text/plain");
  httpResponse.body = HttpResponse::kBadRequestMessage; 
}

#pragma endregion

#pragma region Instance Methods

session::session(boost::asio::io_service& io_service)
    : socket_(io_service)
{
}

tcp::socket& session::socket() 
{
  return socket_;
}

void session::start()
{
  socket_.async_read_some(boost::asio::buffer(_buffer, buffer_length),
      boost::bind(&session::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
{
  if (!error)
  {
    //std::cout<<"handle_read start"<< std::endl;
    int buffer_len = sizeof(_buffer);
    input_.assign(_buffer, _buffer + bytes_transferred);
    if (bytes_transferred < buffer_length) {
      
      string str(input_.begin(), input_.end());
      //std::cout<<"bytes_transferredstd < buffer_len -1"<<std::endl;
      bool success = httpRequest_.parse(str);
      std::cout << str;

      if (success) {
        session::handleGoodRequest(httpRequest_, httpResponse_);
      } else {
        session::handleBadRequest(httpRequest_, httpResponse_);
      }
      
      string res = httpResponse_.to_string();
  
      const char* chars = res.c_str();

      std::cout<<res<< std::endl;
      boost::asio::async_write(socket_,
        boost::asio::buffer(chars, res.length()),
        boost::bind(&session::handle_write, this,
          boost::asio::placeholders::error));
    } else {
      socket_.async_read_some(boost::asio::buffer(_buffer, buffer_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }
  }
  else
  {
    //std::cout<<"handle_read error"<< std::endl;
    delete this;
  }
}

void session::handle_write(const boost::system::error_code& error)
{
  if (!error)
  { 
    //std::cout<<"handle_write done"<< std::endl;
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
  }
  else
  {
    //std::cout<<"handle_write error"<< std::endl;
    delete this;
  }
}

#pragma endregion

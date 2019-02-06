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
#include "handler_manager.h"
#include "logger.h"

// refactored session class from server_main.cc
// part of the codes adapted or inspired from following urls:
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_parser.cpp
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp

#pragma region Static Methods

HttpResponse session::handle_bad_request() {
  HttpResponse res;
  res.version = "HTTP/1.1";
  res.status_code = 400;
  res.headers.push_back("Content-Type: text/plain");
  res.body = HttpResponse::kBadRequestMessage;
  return res;
}

#pragma endregion

#pragma region Instance Methods

session::session(boost::asio::io_service& io_service, HandlerManager* handlerManager)
    : socket_(io_service),
      handlerManager_(handlerManager) {
}

tcp::socket& session::socket() {
  return socket_;
}

void session::start() {
  socket_.async_read_some(boost::asio::buffer(_buffer, buffer_length),
      boost::bind(&session::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  BOOST_LOG_SEV(Logger::get(), INFO)
    << "Established connection with: "
    << socket().remote_endpoint().address().to_string();
}

void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred) {
  string client_ip = socket().remote_endpoint().address().to_string();

  if (!error) {
    BOOST_LOG_SEV(Logger::get(), INFO)
      << "Starting to read bytes from "
      << client_ip;
    int buffer_len = sizeof(_buffer);
    input_.assign(_buffer, _buffer + bytes_transferred);
    if (bytes_transferred < buffer_length) {
      string str(input_.begin(), input_.end());
      BOOST_LOG_SEV(Logger::get(), INFO)
        << "Read " << buffer_len - 1 << " bytes from " << client_ip;
      BOOST_LOG_SEV(Logger::get(), INFO)
        << "Raw request from " << client_ip << ":\n" << str;
      HttpRequest req;
      HttpResponse res;
      bool success = req.parse(str);

      if (success) {
        BOOST_LOG_SEV(Logger::get(), INFO)
          << "Handling valid request from " << client_ip;
        res = handlerManager_->handle_request(req);
      } else {
        BOOST_LOG_SEV(Logger::get(), INFO)
          << "Handling bad request from " << client_ip;
        res = session::handle_bad_request();
      }
      string responseStr = res.to_string();
      const char* chars = responseStr.c_str();
      BOOST_LOG_SEV(Logger::get(), INFO)
        << "Sending a response to " << client_ip
        << " of size " << responseStr.length() << " bytes";
      boost::asio::async_write(socket_,
        boost::asio::buffer(chars, responseStr.length()),
        boost::bind(&session::handle_write, this,
          boost::asio::placeholders::error));
    } else {
      socket_.async_read_some(boost::asio::buffer(_buffer, buffer_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }
  } else {
    BOOST_LOG_SEV(Logger::get(), ERROR)
      << "Failed to read bytes from " << client_ip
      << ": " << error.message();
    delete this;
  }
}

void session::handle_write(const boost::system::error_code& error) {
  string client_ip = socket().remote_endpoint().address().to_string();

  if (!error) {
    BOOST_LOG_SEV(Logger::get(), INFO)
      << "Successfully wrote to " << client_ip
      << ". Shutting down connection.";
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
  } else {
    BOOST_LOG_SEV(Logger::get(), ERROR)
      << "Failed to write bytes to " << client_ip
      << ": " << error.message();
    delete this;
  }
}

#pragma endregion

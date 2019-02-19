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

#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "http_request.h"
#include "http_response.h"
#include "handler_manager.h"
#include "logger.h"
#include "location.h"
#include "config_parser.h"
#include "data_store.h"

// refactored session class from server_main.cc
// part of the codes adapted or inspired from following urls:
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_parser.cpp
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp

#pragma region Instance Methods

session::session(boost::asio::io_service& io_service,
                 LocationMap* locationMap,
                 const string& rootPath)
  : socket_(io_service),
    locationMap_(locationMap),
    rootPath_(rootPath) {
  dataStore_ = &DataStore::getDataStore();
}

session::session(boost::asio::io_service& io_service,
                 LocationMap* locationMap,
                 const string& rootPath,
                 DataStore* dataStore)
  : socket_(io_service),
    locationMap_(locationMap),
    rootPath_(rootPath),
    dataStore_(dataStore) {}

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
      unique_ptr<HttpResponse> res;
      bool success = req.parse(str);
      unique_ptr<Handler> handler;

      if (success) {
        BOOST_LOG_SEV(Logger::get(), INFO)
          << "Handling valid request from " << client_ip;
        LocationInfo* locInfo = LocationUtils::getLongestMatchingLocation(req.target, *locationMap_);

        if (locInfo) {
          string regLocation;
          locInfo->blockConfig->getTopLevelStatement("location", regLocation);
          BOOST_LOG_SEV(Logger::get(), INFO)
            << "Requested location " << req.target
            << " is served by handler registered at " << regLocation;
          BOOST_LOG_SEV(Logger::get(), INFO)
            << "Handler type is '" << locInfo->handlerType << "'";
          handler = HandlerManager::createByName(locInfo->handlerType,
                                                *locInfo->blockConfig,
                                                rootPath_);
        } else {
          handler = HandlerManager::createByName("404", NginxConfig(), rootPath_);
        }
      } else {
        BOOST_LOG_SEV(Logger::get(), INFO)
          << "Handling bad request from " << client_ip;
        handler = HandlerManager::createByName("bad_request", NginxConfig(), rootPath_);
      }
      res = handler->handle_request(req);

      string responseStr = res->to_string();
      const char* chars = responseStr.c_str();
      BOOST_LOG_SEV(Logger::get(), INFO)
        << "Sending a response to " << client_ip
        << " of size " << responseStr.length() << " bytes";
      boost::asio::async_write(socket_,
        boost::asio::buffer(chars, responseStr.length()),
        boost::bind(&session::handle_write, this,
          boost::asio::placeholders::error));

      dataStore_->recordRequest(req.target, res->status_code);
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
    // TODO(nurymka): It is actually not the case that a connection always shuts
    // down here.
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

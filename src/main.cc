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
#include <unordered_map>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "server.h"
#include "logger.h"
#include "config_parser.h"
#include "handler.h"
#include "handler_manager.h"

using namespace std;

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      BOOST_LOG_SEV(Logger::get(), ERROR) << "<error>: Wrong number of arguments for server initialization.\n";
      // std::cerr << "Usage: server <port>\n";
      return 1;
    }

    // copied code from Assignment 1 to find out the portNumber from target config file
    NginxConfigParser config_parser;
    NginxConfig config;
    if (config_parser.Parse(argv[1], &config) == false) {
      BOOST_LOG_SEV(Logger::get(), ERROR) << "<error>: Error encountered parsing config file.\n";
    }
    int16_t portNumber = (int16_t)config.getPort();
    BOOST_LOG_SEV(Logger::get(), INFO) << "<info>: Port number is: " << portNumber;

    unordered_map<string, HandlerMaker*> targetToHandler = config.getTargetToHandler();
    HandlerManager* handlerManager = new HandlerManager(targetToHandler);
    boost::asio::io_service io_service;
    // std::cout<<"starting service at port"<< std::endl;
    // std::cout<<portNumber<< std::endl;
    server s(io_service, portNumber, handlerManager);

    io_service.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

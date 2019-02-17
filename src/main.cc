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
#include <csignal>
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

void interruptHandler(int signum) {
  BOOST_LOG_SEV(Logger::get(), INFO)
    << "Interrupt signal (" << signum << ") received. Terminating...";

  exit(signum);
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      BOOST_LOG_SEV(Logger::get(), CRITICAL)
        << "Wrong number of arguments for server initialization.\n"
        << "Usage: server <port>\n";
      return 1;
    }

    signal(SIGINT, interruptHandler);
    // copied code from Assignment 1 to find out the portNumber from target config file
    NginxConfigParser config_parser;
    NginxConfig config;
    if (!config_parser.Parse(argv[1], &config)) {
      BOOST_LOG_SEV(Logger::get(), ERROR)
        << "Error encountered parsing config file.";
    }
    int16_t portNumber = config.getPort();
    string rootPath = config.getRootPath();

    unordered_map<string, HandlerMaker*> targetToHandler = config.getTargetToHandler();
    HandlerManager* handlerManager = new HandlerManager(targetToHandler);
    boost::asio::io_service io_service;

    server s(io_service, portNumber, rootPath, handlerManager);
    io_service.run();
  }
  catch (std::exception& e) {
    BOOST_LOG_SEV(Logger::get(), CRITICAL)
      << "Exception: " << e.what();
  }

  return 0;
}

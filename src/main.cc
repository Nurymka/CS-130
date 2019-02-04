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
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <unordered_map>
#include "server.h"
#include "session.h"
#include "config_parser.h"
#include "handler.h"
#include "handler_manager.h"


using namespace std;

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

void log_init()
{

  logging::add_file_log(
    keywords::file_name = "server_%Y%m%d.log",
    keywords::rotation_size = 10 * 1024 * 1024,
    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0));

  logging::add_console_log(std::cout, keywords::format = ">> %Message%");

}

int main(int argc, char* argv[])
{
  log_init();
  logging::add_common_attributes();
  src::severity_logger< severity_level > lg = server_log::get();

  try
  {

    if (argc != 2)
    {
      BOOST_LOG_SEV(lg, error) << "Wrong number of arguments for server initialization.\n";
      //std::cerr << "Usage: server <port>\n";
      return 1;
    }

    // copied code from Assignment 1 to find out the portNumber from target config file
    NginxConfigParser config_parser;
    NginxConfig config;
    if (config_parser.Parse(argv[1], &config) == false)
    {
      BOOST_LOG_SEV(lg, error) << "Error encountered parsing config file.\n";
    }
    short portNumber = (short)config.getPort();
    
    BOOST_LOG_SEV(lg, info) << "Port number is: " << portNumber;

    unordered_map<string, Handler*> targetToHandler = config.getTargetToHandler();
    HandlerManager* handlerManager = new HandlerManager(targetToHandler);
    boost::asio::io_service io_service;
    //std::cout<<"starting service at port"<< std::endl;
    //std::cout<<portNumber<< std::endl;
    server s(io_service, portNumber, handlerManager);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

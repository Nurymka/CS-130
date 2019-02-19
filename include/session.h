#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "http_request.h"
#include "http_response.h"
#include "handler_manager.h"
#include "location.h"
#include "data_store.h"

#ifndef SESSION_H_
#define SESSION_H_

using boost::asio::ip::tcp;
using namespace std;

const int buffer_length = 1024;

class session {
 public:
  session(boost::asio::io_service& io_service,
          LocationMap* locationMap,
          const string& rootPath);
  session(boost::asio::io_service& io_service,
          LocationMap* locationMap,
          const string& rootPath,
          DataStore* dataStore_);  // to inject mock in case of testing
  tcp::socket& socket();
  void start();
  static unique_ptr<HttpResponse> handle_bad_request();

 private:
  void handle_read(const boost::system::error_code& error,
  size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

  tcp::socket socket_;
  char _buffer[buffer_length];
  vector<char> input_;

  LocationMap* locationMap_;
  string rootPath_;
  DataStore* dataStore_;
};

#endif  // SESSION_H_

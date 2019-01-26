#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "http_request.h"
#include "http_response.h"

#ifndef SESSION_H
#define SESSION_H

using boost::asio::ip::tcp;
using namespace std;

const int buffer_length = 1024;

class session {
  public:
    session(boost::asio::io_service& io_service);
    tcp::socket& socket();
    void start();
    HttpRequest httpRequest;
    HttpResponse httpResponse;
    
  private:
    void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);

    void handle();
    void handleBadRequest();
    tcp::socket socket_;

    char _buffer[buffer_length];
    vector<char> input;
};

#endif

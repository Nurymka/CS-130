#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "session.h"
#include "handler_manager.h"
#include "location.h"

#ifndef SERVER_H_
#define SERVER_H_

using boost::asio::ip::tcp;

class server {
 public:
    server(boost::asio::io_service& io_service,
          int16_t port,
          const string& rootPath,
          LocationMap locationMap);
    ~server();

    // To initialize a thread pool, must call server::run_io_service() instead
    // of io_service.run() directly.
    void run_io_service();
 private:
    void start_accept();
    void handle_accept(session* new_session,
      const boost::system::error_code& error);
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;

    LocationMap locationMap_;
    int16_t port_;
    string rootPath_;

    const size_t kNumWorkerThreads = 15;
    unique_ptr<boost::thread[]> workerThreads_;
};

#endif  // SERVER_H_

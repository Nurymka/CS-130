#include "reverse_proxy_handler.h"
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <string>
#include "location.h"

using boost::asio::ip::tcp;

Handler* ReverseProxyHandler::create(const NginxConfig& config,
                               const string& root_path) {
  return new ReverseProxyHandler(config, root_path);
}

ReverseProxyHandler::ReverseProxyHandler(const NginxConfig& config, const string& root_path) {
  serverRootPath_ = root_path;
  portNum_ = "80";  // by default

  location_ = "/";
  for (auto statement : config.statements_) {
    // get location
    if (statement->tokens_.size() == 2 && statement->tokens_[0] == "location") {
      location_ = statement->tokens_[1];
    } else if (statement->tokens_.size() == 2 && statement->tokens_[0] == "root") {
      destinationPath_ = statement->tokens_[1];

      // stripping protocol info if found
      std::size_t protocol_pos = destinationPath_.find("//");
      if (protocol_pos != std::string::npos) {
        const int len_of_slashes = 2;
        destinationPath_ = destinationPath_.substr(protocol_pos + len_of_slashes);
      }

      // stripping port number if found
      std::size_t port_pos = destinationPath_.find(":");
      if (port_pos != std::string::npos) {
        portNum_ = destinationPath_.substr(port_pos + 1);
        destinationPath_ = destinationPath_.substr(0, port_pos);
      }
    }
  }
}

unique_ptr<HttpResponse> ReverseProxyHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;
  bool status_redirect = true;
  while (status_redirect) {
    // make connection
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(destinationPath_, portNum_);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end) {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }

    std::string new_target = req.target;
    std::size_t pref_loc = req.target.find(serverRootPath_);
    if (pref_loc != std::string::npos && pref_loc == 0) {
      new_target = new_target.substr(sizeof(serverRootPath_));
    }

    // getting rid of root location eg. /ucla from /ucla/about
    if (new_target == location_) {
      // std::cout << "/ucla" << std::endl;
      new_target = "/";
    } else {
      // std::cout << "/ucla/sometihing" << std::endl;
      new_target = new_target.substr(1);
      new_target = new_target.substr(new_target.find('/'));
    }
    // adding "/" in the back of target
    if (new_target.back() != '/') {
      new_target += "/";
    }

    boost::asio::streambuf new_req;
    std::ostream req_stream(&new_req);
    req_stream << req.method << " " << new_target << " " << req.version << "\r\n";
    req_stream << "Host: " << destinationPath_ << "\r\n";
    req_stream << "Connection: close\r\n\r\n";

    boost::asio::write(socket, new_req);
    // std::cout << "socket written" <<std::endl;

    // read from socket
    // https://stackoverflow.com/questions/38419969/why-boostasioasync-read-completion-
    //         sometimes-executed-with-bytes-transferred
    std::string soc_resp;
    boost::asio::streambuf buf;
    boost::system::error_code ec;
    std::size_t bytes_read;
    while (bytes_read = boost::asio::read(socket, buf, boost::asio::transfer_at_least(1), ec)) {
      std::string data_read = std::string(boost::asio::buffers_begin(buf.data()),
                                          boost::asio::buffers_begin(buf.data()) + bytes_read);
      soc_resp += data_read;
      buf.consume(bytes_read);
    }
    // checking if redirected and ok status
    bool status_ok = true;
    if (soc_resp.substr(0, 10) != "HTTP/1.1 3") {
      status_redirect = false;
      if (soc_resp.substr(0, 10) != "HTTP/1.1 2") {
        status_ok = false;
      }
    }
    // std::cout << "status : " << status_302 << std::endl;
    // Constants to clearify what's added
    const std::size_t CRLF_size = 4;
    const int len_of_code = 3;
    const int len_of_rn = 2;
    const int len_of_loc_name = 8;
    bool first_hdr = true;

    // socket respond: separate header and contents
    std::size_t hdr_pos_end = soc_resp.find("\r\n\r\n");
    // std::cout << soc_resp << std::endl;
    std::string res_body = soc_resp.substr(hdr_pos_end + CRLF_size);
    std::string res_hdr = soc_resp.substr(0, hdr_pos_end + 2);

    int res_code;

    // if status is 302
    if (status_redirect == true) {
      for (;;) {
        std::size_t first_hdr_end = soc_resp.find("\r\n");
        if (first_hdr_end != std::string::npos) {
          if (first_hdr) {
            // get status code
            std::string res_code_string = res_hdr.substr(res_hdr.find(" ") + 1, len_of_code);
            // std::cout << "res code" << res_code_string << std::endl;

            std::stringstream code(res_code_string);
            code >> res_code;
            res->status_code = res_code;
            // make it false so it examines other header
            first_hdr = false;
          } else if (soc_resp.substr(0, len_of_loc_name) == "Location") {
            // location found
            res->headers.push_back(res_hdr.substr(0, first_hdr_end));
            break;
          }
        } else {
          res->body = res_body;
          break;
        }
      }
    } else if (status_ok == true) {
      // going through the header
      for (;;) {
        std::size_t first_hdr_end = res_hdr.find("\r\n");
        if (first_hdr_end != std::string::npos) {
          // if the first header with status code
          if (first_hdr) {
             // get status code
            std::string res_code_string = res_hdr.substr(res_hdr.find(" ") + 1, len_of_code);
            std::stringstream code(res_code_string);
            code >> res_code;
            res->status_code = res_code;
            // std::cout << "status_code 200? " << res_code_string << ":" << res_code << std::endl;
            // make it false so it examines other header
            first_hdr = false;
            res_hdr = res_hdr.substr(first_hdr_end + 2);
          } else {
            res->headers.push_back(res_hdr.substr(0, first_hdr_end));
            res_hdr = res_hdr.substr(first_hdr_end + 2);
            // std::cout << res_hdr << std::endl;
          }
        } else {
          res->body = res_body;
          break;
        }
      }
    } else {
      //status not found
      res->status_code = 400;
    }
  }  // end while
  return res;
}

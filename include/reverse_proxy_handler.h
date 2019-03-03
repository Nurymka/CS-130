#include <string>
#include <unordered_map>

#include "config_parser.h"
#include "handler.h"
#include "http_request.h"
#include "http_response.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

#ifndef REVERSE_PROXY_HANDLER_H_
#define REVERSE_PROXY_HANDLER_H_

class ReverseProxyHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);

  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);

 private:
  ReverseProxyHandler(const NginxConfig& config, const string& root_path);
  string serverRootPath_;
  string destinationPath_;
  string location_;
  string portNum_;
};

#endif  // REVERSE_PROXY_HANDLER_H_

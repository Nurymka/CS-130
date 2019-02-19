#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"

#ifndef HANDLER_H_
#define HANDLER_H_

class Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  virtual unique_ptr<HttpResponse> handle_request(const HttpRequest& req) = 0;
};

#endif  // HANDLER_H_

#include "handler.h"
#include "http_request.h"
#include "http_response.h"

#ifndef ECHO_HANDLER_H_
#define ECHO_HANDLER_H_

class EchoHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);
};

#endif  // ECHO_HANDLER_H_

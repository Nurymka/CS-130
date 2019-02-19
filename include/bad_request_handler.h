#include "handler.h"
#include "http_request.h"
#include "http_response.h"

#ifndef BAD_REQUEST_HANDLER_H_
#define BAD_REQUEST_HANDLER_H_

class BadRequestHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);
};

#endif  // BAD_REQUEST_HANDLER_H_

#include "handler.h"
#include "http_request.h"
#include "http_response.h"

#ifndef NOT_FOUND_HANDLER_H_
#define NOT_FOUND_HANDLER_H_

class NotFoundHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);
};

#endif  // NOT_FOUND_HANDLER_H_

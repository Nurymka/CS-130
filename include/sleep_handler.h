#include "handler.h"

#ifndef SLEEP_HANDLER_H_
#define SLEEP_HANDLER_H_

class SleepHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);
};

#endif  // SLEEP_HANDLER_H_

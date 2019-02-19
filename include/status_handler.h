#include "handler.h"
#include "http_request.h"
#include "http_response.h"
#include "data_store.h"

#ifndef STATUS_HANDLER_H_
#define STATUS_HANDLER_H_

class StatusHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  static Handler* create(const NginxConfig& config,
                         const string& root_path,
                         DataStore* dataStore);  // to inject mock for testing
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);

 private:
  StatusHandler();
  explicit StatusHandler(DataStore* dataStore);  // to inject mock for testing

  DataStore* dataStore_;
};

#endif  // STATUS_HANDLER_H_

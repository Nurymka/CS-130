#include <unistd.h>
#include "sleep_handler.h"

Handler* SleepHandler::create(const NginxConfig& config, const string& root_path) {
  return new SleepHandler();
}

unique_ptr<HttpResponse> SleepHandler::handle_request(const HttpRequest& req) {
  usleep(10 * 1000000);  // sleeps thread for 10 sec
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;
  res->status_code = 200;
  return res;
}

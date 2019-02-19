#include <memory>
#include "not_found_handler.h"
#include "http_request.h"
#include "http_response.h"

using namespace std;

Handler* NotFoundHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new NotFoundHandler();
}

unique_ptr<HttpResponse> NotFoundHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = "HTTP/1.1";
  res->status_code = 404;
  return res;
}

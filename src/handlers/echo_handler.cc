#include <memory>
#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"

using namespace std;

Handler* EchoHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new EchoHandler();
}

unique_ptr<HttpResponse> EchoHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;
  res->status_code = 200;
  res->headers.push_back("Content-Type: text/plain");
  res->body = req.to_string();
  return res;
}

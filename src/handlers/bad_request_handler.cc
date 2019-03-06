#include <memory>
#include "bad_request_handler.h"
#include "http_request.h"
#include "http_response.h"

using namespace std;

Handler* BadRequestHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new BadRequestHandler();
}

unique_ptr<HttpResponse> BadRequestHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = "HTTP/1.1";
  res->status_code = 400;
  res->headers.push_back("Content-Type: text/plain");
  res->body = HttpResponse::kBadRequestMessage;
  return res;
}

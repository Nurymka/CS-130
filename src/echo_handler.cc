#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"



HttpResponse EchoHandler::handle_request(HttpRequest req) {
  HttpResponse res;
  res.version = req.version;
  res.status_code = 200;
  res.headers.push_back("Content-Type: text/plain");
  res.body = req.to_string();
  
  return res;
}
#include "handler_manager.h"
#include "handler.h"
#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"


// TODO: Inspect req.path and determine which handler to use
HttpResponse HandlerManager::handle_request(HttpRequest req) {
  EchoHandler echoHandler;
  return echoHandler.handle_request(req);
}

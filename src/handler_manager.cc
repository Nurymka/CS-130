#include "handler_manager.h"
#include "handler.h"
#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"

HandlerManager::HandlerManager(unordered_map<string, Handler*> targetToHandler)
  : targetToHandler_(targetToHandler)
{
}

HandlerManager::~HandlerManager() {
  for (auto i : targetToHandler_) {
    delete i.second;
  }
}

// TODO: Inspect req.path and determine which handler to use
HttpResponse HandlerManager::handle_request(HttpRequest req) {
  if (targetToHandler_.find(req.target) != targetToHandler_.end()) {
    return targetToHandler_[req.target]->handle_request(req);
  }
  else {
    // TODO: return 404 not found
    HttpResponse res;
    return res;
  }
}

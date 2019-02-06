#include "handler_manager.h"
#include "handler.h"
#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"

HandlerManager::HandlerManager(unordered_map<string, HandlerMaker*> targetToHandler)
  : targetToHandler_(targetToHandler) {
}

HandlerManager::~HandlerManager() {
  for (auto i : targetToHandler_) {
    delete i.second;
  }
}

// TODO(nate): Inspect req.path and determine which handler to use
HttpResponse HandlerManager::handle_request(HttpRequest req) {
  HandlerMaker* maker = NULL;
  for (unordered_map<string, HandlerMaker*>::iterator
    it = targetToHandler_.begin(); it != targetToHandler_.end(); ++it) {
    if (req.target.rfind(it->first, 0) == 0) {
      maker = it->second;
      break;
    }
  }
  if (maker != NULL) {
    Handler* handler = maker->create();
    HttpResponse res = handler->handle_request(req);
    free(handler);
    return res;
  } else {
    HttpResponse res;
    res.version = req.version;
    res.status_code = 404;
    return res;
  }
}

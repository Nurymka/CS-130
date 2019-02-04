#include "handler_manager.h"
#include "handler.h"
#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"

HandlerManager::HandlerManager(unordered_map<string, HandlerMaker*> targetToHandler)
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
  HandlerMaker* maker = NULL;
  for(unordered_map<string, HandlerMaker*>::iterator it = targetToHandler_.begin(); it != targetToHandler_.end(); ++it) {
    if (req.target.rfind(it->first, 0) == 0) {
      maker = it->second;
      break;
    }
  }
  if (maker != NULL) {
    Handler* handler = maker->create();
    HttpResponse res = handler->handle_request(req);
    //TODO: eed fix null pointer
    //free(handler);
    return res;
  }
  else {
    // TODO: return 404 not found
    HttpResponse res;
    res.status_code = 404;
    return res;
  }
}

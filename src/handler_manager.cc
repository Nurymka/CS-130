#include "handler_manager.h"
#include "handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"
#include "bad_request_handler.h"

unique_ptr<Handler> HandlerManager::createByName(const string& name,
                                                 const NginxConfig& config,
                                                 const string& root_path) {
  if (name == "echo") {
    return unique_ptr<Handler>(EchoHandler::create(config, root_path));
  } else if (name == "static") {
    return unique_ptr<Handler>(StaticHandler::create(config, root_path));
  } else if (name == "bad_request") {
    return unique_ptr<Handler>(BadRequestHandler::create(config, root_path));
  } else {
    return unique_ptr<Handler>(NotFoundHandler::create(config, root_path));
  }
}

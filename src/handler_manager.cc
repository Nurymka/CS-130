#include "handler_manager.h"
#include "handler.h"
#include "echo_handler.h"
#include "static_handler.h"

unique_ptr<Handler> HandlerManager::createByName(const string& name,
                                                 const NginxConfig& config,
                                                 const string& root_path) {
  if (name == "echo") {
    return unique_ptr<Handler>(EchoHandler::create(config, root_path));
  } else if (name == "static") {
    return unique_ptr<Handler>(StaticHandler::create(config, root_path));
  } else {
    // TODO(nurymka): figure out what to do by default
    return unique_ptr<Handler>(EchoHandler::create(config, root_path));
  }
}

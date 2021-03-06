#include "handler_manager.h"
#include "handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"
#include "bad_request_handler.h"
#include "status_handler.h"
#include "reverse_proxy_handler.h"
#include "sleep_handler.h"
#include "new_meme_handler.h"
#include "view_meme_handler.h"
#include "list_meme_handler.h"
#include "delete_meme_handler.h"

unique_ptr<Handler> HandlerManager::createByName(const string& name,
                                                 const NginxConfig& config,
                                                 const string& root_path) {
  if (name == "echo") {
    return unique_ptr<Handler>(EchoHandler::create(config, root_path));
  } else if (name == "static") {
    return unique_ptr<Handler>(StaticHandler::create(config, root_path));
  } else if (name == "bad_request") {
    return unique_ptr<Handler>(BadRequestHandler::create(config, root_path));
  } else if (name == "status") {
    return unique_ptr<Handler>(StatusHandler::create(config, root_path));
  } else if (name == "proxy") {
    return unique_ptr<Handler>(ReverseProxyHandler::create(config, root_path));
  } else if (name == "sleep") {
    return unique_ptr<Handler>(SleepHandler::create(config, root_path));
  } else if (name == "new_meme") {
    return unique_ptr<Handler>(NewMemeHandler::create(config, root_path));
  } else if (name == "view_meme") {
    return unique_ptr<Handler>(ViewMemeHandler::create(config, root_path));
  } else if (name == "list_meme") {
    return unique_ptr<Handler>(ListMemeHandler::create(config, root_path));
  } else if (name == "delete_meme") {
    return unique_ptr<Handler>(DeleteMemeHandler::create(config, root_path));
  } else {
    return unique_ptr<Handler>(NotFoundHandler::create(config, root_path));
  }
}

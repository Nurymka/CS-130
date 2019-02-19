#include <unordered_map>
#include <string>
#include "http_request.h"
#include "http_response.h"
#include "handler.h"

#ifndef HANDLER_MANAGER_H_
#define HANDLER_MANAGER_H_

using namespace std;

class HandlerManager {
 public:
  unique_ptr<Handler> createByName(const string& name,
                                   const NginxConfig& config,
                                   const string& root_path);
};

#endif  // HANDLER_MANAGER_H_

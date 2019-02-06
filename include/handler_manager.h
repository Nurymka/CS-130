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
    explicit HandlerManager(unordered_map<string, Handler*> targetToHandler);
    ~HandlerManager();
    HttpResponse handle_request(HttpRequest req);

 private:
    // TODO(nate): initialize target to handler mapping from config parsing
    unordered_map<string, Handler*> targetToHandler_;
};

#endif  // HANDLER_MANAGER_H_

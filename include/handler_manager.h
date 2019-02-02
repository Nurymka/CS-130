#include <map>
#include <string>
#include "http_request.h"
#include "http_response.h"
#include "handler.h"

#ifndef HANDLER_MANAGER_H 
#define HANDLER_MANAGER_H 

using namespace std;

class HandlerManager {
  public:
    HttpResponse handle_request(HttpRequest req);
  
  private:
    // TODO: initialize target to handler mapping from config parsing
    map<string, Handler*> targetToHandler;
};

#endif
#include "handler.h"
#include "http_request.h"
#include "http_response.h"

#ifndef STATIC_HANDLER_H 
#define STATIC_HANDLER_H 

class StaticHandler : public Handler {
  public:
    HttpResponse handle_request(HttpRequest req);
    StaticHandler(std::string root, std::string prefix): _root(root), _prefix(prefix) {}
  private:
    std::string _root;
    std::string _prefix;
};

class StaticHandlerMaker : public HandlerMaker {
  public:
    Handler* create();
    StaticHandlerMaker(std::string root, std::string prefix): _root(root), _prefix(prefix) {}

  private:
    std::string _root;
    std::string _prefix;
};

#endif
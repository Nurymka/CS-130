#include "http_request.h"
#include "http_response.h"

#ifndef HANDLER_H 
#define HANDLER_H 

class Handler {
  public:
    virtual HttpResponse handle_request(HttpRequest req) = 0;
};

#endif
#include "handler.h"
#include "http_request.h"
#include "http_response.h"

#ifndef ECHO_HANDLER_H 
#define ECHO_HANDLER_H 

class EchoHandler : public Handler {
  public:
    HttpResponse handle_request(HttpRequest req);
};

#endif
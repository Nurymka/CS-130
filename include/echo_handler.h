#include "handler.h"
#include "http_request.h"
#include "http_response.h"

#ifndef ECHO_HANDLER_H_
#define ECHO_HANDLER_H_

class EchoHandler : public Handler {
 public:
    HttpResponse handle_request(HttpRequest req);
};

class EchoHandlerMaker : public HandlerMaker {
 public:
    Handler* create();
};

#endif  // ECHO_HANDLER_H_

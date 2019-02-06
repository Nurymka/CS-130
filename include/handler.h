#include "http_request.h"
#include "http_response.h"

#ifndef HANDLER_H_
#define HANDLER_H_

class Handler {
 public:
    virtual HttpResponse handle_request(HttpRequest req) = 0;
};

class HandlerMaker {
 public:
    virtual Handler* create() = 0;
};

#endif  // HANDLER_H_

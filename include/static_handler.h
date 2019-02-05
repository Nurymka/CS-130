#include <unordered_map>
#include <string>
#include "handler.h"
#include "http_request.h"
#include "http_response.h"

using namespace std;

#ifndef STATIC_HANDLER_H 
#define STATIC_HANDLER_H 

// http://docs.w3cub.com/http/basics_of_http/mime_types/complete_list_of_mime_types/
const unordered_map<string, string> EXT_TO_CONTENT_TYPE = {
  { "html", "text/html" },
  { "jpg", "image/jpeg" },
  { "jpeg", "image/jpeg" },
  { "png", "image/png" },
  { "zip", "application/zip"}
};

const string DEFAULT_CONTENT_TYPE = "text/plain";

class StaticHandler : public Handler {
  public:
    StaticHandler(string root, string prefix): _root(root), _prefix(prefix) {}
    string get_mime_type(string filename);
    HttpResponse handle_request(HttpRequest req);
  private:
    string _root;
    string _prefix;
};

class StaticHandlerMaker : public HandlerMaker {
  public:
    Handler* create();
    StaticHandlerMaker(string root, string prefix): _root(root), _prefix(prefix) {}

  private:
    string _root;
    string _prefix;
};

#endif
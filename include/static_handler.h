#include <unordered_map>
#include <string>

#include "config_parser.h"
#include "handler.h"
#include "http_request.h"
#include "http_response.h"

using namespace std;

#ifndef STATIC_HANDLER_H_
#define STATIC_HANDLER_H_

// http://docs.w3cub.com/http/basics_of_http/mime_types/complete_list_of_mime_types/
const unordered_map<string, string> EXT_TO_CONTENT_TYPE = {
  { "html", "text/html" },
  { "jpg", "image/jpeg" },
  { "jpeg", "image/jpeg" },
  { "png", "image/png" },
  { "zip", "application/zip"},
  { "css", "text/css" }
};

const char DEFAULT_CONTENT_TYPE[] = "text/plain";

class StaticHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);

  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);
  static string get_mime_type(string filename);

 private:
  StaticHandler(const NginxConfig& config, const string& root_path);
  string serverRootPath_;
  string staticRootPath_;
  string location_;

  // in case "file" keyword is used, the servlet returns file at serverRootPath + exactFilePath;
  string exactFilePath_;
};

#endif  // STATIC_HANDLER_H_

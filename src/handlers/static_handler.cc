#include <iostream>
#include <fstream>

#include "static_handler.h"
#include "location.h"

using namespace std;

Handler* StaticHandler::create(const NginxConfig& config,
                               const string& root_path) {
  return new StaticHandler(config, root_path);
}

StaticHandler::StaticHandler(const NginxConfig& config, const string& root_path) {
  // TODO(nurymka): force root & location statements inside the block config
  serverRootPath_ = root_path;
  if (config.getTopLevelStatement("root", staticRootPath_))
    staticRootPath_ = LocationUtils::extractPathOnly(staticRootPath_);
  if (config.getTopLevelStatement("location", location_))
    location_ = LocationUtils::extractPathOnly(location_);
  if (config.getTopLevelStatement("file", exactFilePath_))
    exactFilePath_ = LocationUtils::extractPathOnly(exactFilePath_);
}

string StaticHandler::get_mime_type(string filename) {
  int index = filename.rfind(".");
  if (index != std::string::npos) {
    string extension = filename.substr(index+1);
    // https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key
    if (EXT_TO_CONTENT_TYPE.find(extension) != EXT_TO_CONTENT_TYPE.end()) {
      return EXT_TO_CONTENT_TYPE.at(extension);
    } else {
      return DEFAULT_CONTENT_TYPE;
    }
  } else {
    return DEFAULT_CONTENT_TYPE;
  }
}

unique_ptr<HttpResponse> StaticHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;

  string file_path;

  if (exactFilePath_.empty()) {
    file_path = LocationUtils::concatPaths(serverRootPath_, staticRootPath_);

    if (req.target.substr(location_.length()).length() <= 1)
      file_path = LocationUtils::concatPaths(file_path, "index.html");
    else
      file_path = LocationUtils::concatPaths(file_path, req.target.substr(location_.length()));
  } else {
    file_path = LocationUtils::concatPaths(serverRootPath_, exactFilePath_);
  }

  // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
  std::ifstream t(file_path);
  if (!t.good()) {
    res->status_code = 404;
  } else {
    std::stringstream buffer;
    buffer << t.rdbuf();

    res->status_code = 200;
    res->headers.push_back("Content-Type: " + get_mime_type(file_path));
    res->body = buffer.str();
  }
  return res;
}

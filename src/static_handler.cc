#include <iostream> 
#include <fstream>

#include "static_handler.h"
#include "http_request.h"
#include "http_response.h"

string StaticHandler::get_mime_type(string filename) {
  int index = filename.rfind(".");
  if (index != std::string::npos) {
    string extension = filename.substr(index+1);
    // https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key
    if (EXT_TO_CONTENT_TYPE.find(extension) != EXT_TO_CONTENT_TYPE.end()) {
      return EXT_TO_CONTENT_TYPE.at(extension);
    }
    else {
      return DEFAULT_CONTENT_TYPE;
    }
  }
  else {
    return DEFAULT_CONTENT_TYPE;
  }
}

HttpResponse StaticHandler::handle_request(HttpRequest req) {
  HttpResponse res;
  res.version = req.version;

  std::string requestTarget = req.target;

  std::string suffix = requestTarget.substr(_prefix.length());
  if(suffix.at(0) == '/'){
    suffix = suffix.substr(1);
  }
  if(_root.at(_root.length() - 1) == '/'){
    _root = _root.substr(0, _root.length() - 1);
  }

  std::string file_path = _root + "/" + suffix;
  
  // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
  std::ifstream t(file_path);
  if (!t.good()) {
    res.status_code = 404;
  }
  else {
    std::stringstream buffer;
    buffer << t.rdbuf();

    res.status_code = 200;
    res.headers.push_back(get_mime_type(file_path));
    res.body = buffer.str();
  }
  return res;
}

Handler* StaticHandlerMaker::create() {
  return new StaticHandler(this->_root, this->_prefix);
}

#include <iostream> 
#include <fstream>

#include "static_handler.h"
#include "http_request.h"
#include "http_response.h"

HttpResponse StaticHandler::handle_request(HttpRequest req) {
  HttpResponse res;

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
  } else {
    std::stringstream buffer;
    buffer << t.rdbuf();

    res.body = buffer.str();
    res.status_code = 200;
  }
  return res;
}

Handler* StaticHandlerMaker::create() {
  return new StaticHandler(this->_root, this->_prefix);
}

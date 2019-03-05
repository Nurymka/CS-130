#include <memory>
#include "view_meme_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"
#include "meme.h"

using namespace std;

Handler* ViewMemeHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new ViewMemeHandler();
}

ViewMemeHandler::ViewMemeHandler() {
  memeDB_ = new MemeDB();
}

ViewMemeHandler::ViewMemeHandler(MemeDB* memeDB) : memeDB_(memeDB) {}

ViewMemeHandler::~ViewMemeHandler() {
  delete memeDB_;
}

// Accepts GET requests with required ID parameters: img_path, top_text, bottom_text
// Returns a 200 response with the requested meme.
unique_ptr<HttpResponse> ViewMemeHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;

  if (req.method == "GET" && req.data.find("id") != req.data.end()) {
    string id = req.data.at("id");
    Meme found_meme = memeDB_->findByID(id);

    if(found_meme != "") {
      //TODO: Package Meme contents into JSON/specified format for HTTP response
      continue;
    } else {
      res->status_code = 400;
    }
  } else {
    res->status_code = 400;
  }
  return res;
}

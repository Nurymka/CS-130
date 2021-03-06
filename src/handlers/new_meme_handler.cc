#include <memory>
#include "new_meme_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"

using namespace std;

Handler* NewMemeHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new NewMemeHandler();
}

NewMemeHandler::NewMemeHandler() {
  memeDB_ = &MemeDB::getMemeDB(); 
}

NewMemeHandler::NewMemeHandler(MemeDB* memeDB) : memeDB_(memeDB) {}

NewMemeHandler::~NewMemeHandler() {
  delete memeDB_;
}

// Accepts POST requests with required parameters: img_path, top_text, bottom_text
// Returns a 201 response with the id of the newly saved meme (JSON formatted) upon
// success, a 400 response upon error.
unique_ptr<HttpResponse> NewMemeHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;

  if (req.method == "POST" &&
      req.data.find("img_path") != req.data.end() &&
      req.data.find("top_text") != req.data.end() &&
      req.data.find("bottom_text") != req.data.end()) {
    string img_path = req.data.at("img_path");
    string top_text = req.data.at("top_text");
    string bottom_text = req.data.at("bottom_text");

    bool completed ;
    string id;
    // update existing meme
    if (req.data.find("update") != req.data.end()) {
      id = req.data.at("update");
      bool updated = memeDB_->update(id, img_path, top_text, bottom_text);
      completed = updated;
    } else { // create new meme
      int id_num = memeDB_->add(img_path, top_text, bottom_text);
      completed = (id_num != -1);
      id = to_string(id_num);
    }
    if (completed) {
      res->status_code = 201;
      res->headers.push_back("Content-Type: application/json");
      res->body = "{ \"id\" : " + id + " }";
    } else {
      res->status_code = 400;
    }
  } else {
    res->status_code = 400;
  }
  return res;
}

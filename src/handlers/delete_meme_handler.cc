#include <memory>
#include "delete_meme_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"

using namespace std;

Handler* DeleteMemeHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new DeleteMemeHandler();
}

DeleteMemeHandler::DeleteMemeHandler() {
  memeDB_ = &MemeDB::getMemeDB(); 
}

DeleteMemeHandler::DeleteMemeHandler(MemeDB* memeDB) : memeDB_(memeDB) {}

// Accepts POST requests with required parameters: id
// Returns a 200 response upon success, a 400 response upon error.
unique_ptr<HttpResponse> DeleteMemeHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;

  if (req.method == "POST" &&
      req.data.find("id") != req.data.end()) {
    string id = req.data.at("id");
    bool completed = memeDB_->remove(id);
    if (completed) {
      res->status_code = 200;
    } else {
      res->status_code = 400;
    }
  } else {
    res->status_code = 400;
  }
  return res;
}

#include <memory>
#include <sstream>
#include "list_meme_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"
#include "meme.h"

using namespace std;

Handler* ListMemeHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new ListMemeHandler();
}

ListMemeHandler::ListMemeHandler() {
  memeDB_ = new MemeDB();
}

ListMemeHandler::ListMemeHandler(MemeDB* memeDB) : memeDB_(memeDB) {}

ListMemeHandler::~ListMemeHandler() {
  delete memeDB_;
}

void ListMemeHandler::setMemeDB(MemeDB* memeDB) {
  memeDB_ = memeDB;
}


// Returns a 200 response with the requested meme.
unique_ptr<HttpResponse> ListMemeHandler::handle_request(const HttpRequest& req) {
  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;

  if (req.method == "GET") {
    string query;
    if (req.data.find("q") != req.data.end()) {
      query = req.data.at("q");
    }
    // retrieve all memes with matching query
    vector<Meme> memes = memeDB_->findAll(query);

    res->status_code = 200;
    // We consolidate the memes into a nice array of JSON objects.
    stringstream meme_list;
    meme_list << "{ \"memes\" : [ ";
    vector<Meme>::iterator it = memes.begin();
    while (it != memes.end()) {
      meme_list << it->toJSON();
      it++;
      if (it == memes.end()) {
        break;
      } else {
        meme_list << ", ";
      }
    }
    meme_list << " ] }";
    res->body = meme_list.str();

  } else {
    res->status_code = 400;
  }
  return res;
}

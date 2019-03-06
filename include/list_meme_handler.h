#include "handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"

#ifndef LIST_MEME_HANDLER_H_
#define LIST_MEME_HANDLER_H_

class ListMemeHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);

 private:
  ListMemeHandler();
  explicit ListMemeHandler(MemeDB* memeDB);
  ~ListMemeHandler();
  MemeDB* memeDB_;
};

#endif  // LIST_MEME_HANDLER_H_
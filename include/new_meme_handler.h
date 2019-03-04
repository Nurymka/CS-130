#include "handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"

#ifndef NEW_MEME_HANDLER_H_
#define NEW_MEME_HANDLER_H_

class NewMemeHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);

 private:
  NewMemeHandler();
  explicit NewMemeHandler(MemeDB* memeDB);
  ~NewMemeHandler();
  MemeDB* memeDB_;
};

#endif  // NEW_MEME_HANDLER_H_

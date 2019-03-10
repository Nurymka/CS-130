#include "handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"

#ifndef DELETE_MEME_HANDLER_H_
#define DELETE_MEME_HANDLER_H_

class DeleteMemeHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);
  void setMemeDB(MemeDB* memeDB);

 private:
  DeleteMemeHandler();
  explicit DeleteMemeHandler(MemeDB* memeDB);
  MemeDB* memeDB_;
};

#endif  // DELETE_MEME_HANDLER_H_

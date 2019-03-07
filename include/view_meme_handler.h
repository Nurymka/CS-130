#include "handler.h"
#include "http_request.h"
#include "http_response.h"
#include "meme_db.h"

#ifndef VIEW_MEME_HANDLER_H_
#define VIEW_MEME_HANDLER_H_

class ViewMemeHandler : public Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const string& root_path);
  unique_ptr<HttpResponse> handle_request(const HttpRequest& req);
  void setMemeDB(MemeDB *memeDB);

 private:
  ViewMemeHandler();
  explicit ViewMemeHandler(MemeDB* memeDB);
  ~ViewMemeHandler();
  MemeDB* memeDB_;
};

#endif  // VIEW_MEME_HANDLER_H_
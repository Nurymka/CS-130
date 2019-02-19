#include <memory>
#include "gtest/gtest.h"
#include "session.h"
#include "http_response.h"

namespace {
class SessionTest: public ::testing::Test {
 protected:
  HttpRequest req;
  unique_ptr<HttpResponse> res;
};

TEST_F(SessionTest, CheckBadRequestHandler) {
  res = session::handle_bad_request();

  string expected_res;
  expected_res += "HTTP/1.1 400 Bad Request\r\n";
  expected_res += "Content-Type: text/plain\r\n";
  expected_res += "Content-Length: 136\r\n";
  expected_res += "\r\n";
  expected_res += "Your client sent an invalid request to the server.\r\n";
  expected_res +=
    "Please check for syntax errors and make sure the client is forming HTTP/1.1 requests";

  EXPECT_EQ(res->to_string(), expected_res);
}
}  // namespace

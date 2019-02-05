#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"

using ::testing::StartsWith;
using ::testing::HasSubstr;
using ::testing::EndsWith;

namespace {
  class EchoHandlerTest : public ::testing::Test {
    protected:
      EchoHandler echoHandler;
      HttpRequest req;
      HttpResponse res;
  };

  TEST_F(EchoHandlerTest, CheckGoodRequestHandler) {
    string input;
    input += "GET / HTTP/1.1\r\n";
    input += "Host: localhost\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "\r\n";

    req.parse(input);
    res = echoHandler.handle_request(req);

    string responseStr = res.to_string();
    EXPECT_THAT(responseStr, StartsWith("HTTP/1.1 200 OK\r\n"));
    EXPECT_THAT(responseStr, HasSubstr("Content-Type: text/plain\r\n"));
    EXPECT_THAT(responseStr, EndsWith(input)); 
  }
}
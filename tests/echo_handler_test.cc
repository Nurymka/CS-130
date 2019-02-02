#include "gtest/gtest.h"
#include "echo_handler.h"
#include "http_request.h"
#include "http_response.h"

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
    
    string expected_res;
    expected_res += "HTTP/1.1 200 OK\r\n";
    expected_res += "Content-Type: text/plain\r\n";
    expected_res += "Content-Length: 73\r\n";
    expected_res += "\r\n";
    expected_res += "GET / HTTP/1.1\r\n";
    expected_res += "Host: localhost\r\n";
    expected_res += "User-Agent: curl/7.58.0\r\n";
    expected_res += "Accept: */*\r\n";
    expected_res += "\r\n";
    
    EXPECT_EQ(res.to_string(), expected_res);
  }
}
#include "gtest/gtest.h"
#include "session.h"
#include "http_response.h"

namespace {
  class SessionTest: public ::testing::Test {
    protected:
      HttpRequest req;
      HttpResponse res;
  };

  TEST_F(SessionTest, CheckGoodRequestHandler) {
    string input;
    input += "GET / HTTP/1.1\r\n";
    input += "Host: localhost\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "\r\n";

    req.parse(input);
    session::handleGoodRequest(req, res);
    
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

  TEST_F(SessionTest, CheckBadRequestHandler) {
    string input;
    input += "hello";
    
    req.parse(input);
    session::handleBadRequest(req, res);

    string expected_res;
    expected_res += "HTTP/1.1 400 Bad Request\r\n";
    expected_res += "Content-Type: text/plain\r\n";
    expected_res += "Content-Length: " + to_string(HttpResponse::kBadRequestMessage.size()) + "\r\n";
    expected_res += "\r\n";
    expected_res += "Your client sent an invalid request to the server.\r\n";
    expected_res += "Please check for syntax errors and make sure the client is forming HTTP/1.1 requests";

    EXPECT_EQ(res.to_string(), expected_res); 
  }
}
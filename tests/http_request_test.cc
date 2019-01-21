#include "gtest/gtest.h"
#include "http_request.h"

namespace {
  class HttpRequestTest: public ::testing::Test {

  protected:
    HttpRequest req;
  };

    TEST_F(HttpRequestTest, CheckValidParsing) {
        string input;
        input += "POST / HTTP/1.1\r\n";
        input += "Host: localhost:8080\r\n";
        input += "User-Agent: curl/7.58.0\r\n";
        input += "Accept: */*\r\n";
        input += "Content-Length: 4\r\n";
        input += "Content-Type: application/x-www-form-urlencoded\r\n";
        input += "\n";
        input += "TEST\r\n";
        
        req.parse(input);
        EXPECT_EQ(req.method, "POST");
        EXPECT_EQ(req.target, "/");
        EXPECT_EQ(req.version, "HTTP/1.1");
        EXPECT_EQ(req.headers.size(), 5);
        EXPECT_EQ(req.body, "TEST\r");
    }

}
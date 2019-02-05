#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "static_handler.h"
#include "http_request.h"
#include "http_response.h"

const string TEST_ROOT = "/usr/src/projects/iceberg-webserver/tests/static_file";

using ::testing::StartsWith;
using ::testing::HasSubstr;
using ::testing::EndsWith;

namespace {
  class StaticHandlerTest: public ::testing::Test {
    protected:
      StaticHandler staticHandler = StaticHandler(TEST_ROOT, "/target");
      HttpRequest req;
      HttpResponse res;
  };

  TEST_F(StaticHandlerTest, TestGetMimeType) {
    EXPECT_EQ(staticHandler.get_mime_type("file.html"), "text/html");
    EXPECT_EQ(staticHandler.get_mime_type("file.jpg"), "image/jpeg");
    EXPECT_EQ(staticHandler.get_mime_type("file.txt"), "text/plain");
    EXPECT_EQ(staticHandler.get_mime_type("file.unknown"), "text/plain");
  }

  TEST_F(StaticHandlerTest, TestGoodRequest) {
    string input;
    input += "GET /target/test.txt HTTP/1.1\r\n";
    input += "Host: localhost\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "\r\n";

    req.parse(input);
    res = staticHandler.handle_request(req);
    
    string responseStr = res.to_string();
    EXPECT_THAT(responseStr, StartsWith("HTTP/1.1 200 OK\r\n"));
    EXPECT_THAT(responseStr, HasSubstr("Content-Type: text/plain\r\n"));
    EXPECT_THAT(responseStr, EndsWith("TEST"));
  }

  TEST_F(StaticHandlerTest, TestBadRequest) {
    string input;
    input += "GET /target/file_does_not_exist HTTP/1.1\r\n";
    input += "Host: localhost\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "\r\n";

    req.parse(input);
    res = staticHandler.handle_request(req);
    
    string responseStr = res.to_string();
    EXPECT_THAT(responseStr, StartsWith("HTTP/1.1 404 Not Found\r\n"));
  }
}
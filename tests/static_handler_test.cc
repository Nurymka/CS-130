#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "static_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"

using ::testing::StartsWith;
using ::testing::HasSubstr;
using ::testing::EndsWith;

namespace {
class StaticHandlerTest: public ::testing::Test {
 protected:
  NginxConfigParser parser;
  NginxConfig config;

  Handler* staticHandler;
  HttpRequest req;
  unique_ptr<HttpResponse> res;

  virtual void SetUp() {
    parser.Parse("config_parser_test_input/static_handler_config", &config);
    shared_ptr<NginxConfig> blockConfig = config.statements_[1]->child_block_;
    staticHandler = StaticHandler::create(*blockConfig, config.getRootPath());
  }

  virtual void TearDown() {
    delete staticHandler;
  }
};

TEST_F(StaticHandlerTest, TestGetMimeType) {
  EXPECT_EQ(StaticHandler::get_mime_type("file.html"), "text/html");
  EXPECT_EQ(StaticHandler::get_mime_type("file.jpg"), "image/jpeg");
  EXPECT_EQ(StaticHandler::get_mime_type("file.txt"), "text/plain");
  EXPECT_EQ(StaticHandler::get_mime_type("file.unknown"), "text/plain");
}

TEST_F(StaticHandlerTest, TestGoodRequest) {
  string input;
  input += "GET /target/test.txt HTTP/1.1\r\n";
  input += "Host: localhost\r\n";
  input += "User-Agent: curl/7.58.0\r\n";
  input += "Accept: */*\r\n";
  input += "\r\n";

  req.parse(input);
  res = staticHandler->handle_request(req);

  string responseStr = res->to_string();
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
  res = staticHandler->handle_request(req);

  string responseStr = res->to_string();
  EXPECT_THAT(responseStr, StartsWith("HTTP/1.1 404 Not Found\r\n"));
}
}  // namespace

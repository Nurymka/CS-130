#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "reverse_proxy_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"

using ::testing::StartsWith;
using ::testing::HasSubstr;
using ::testing::EndsWith;

namespace {
class ReverseProxyHandlerTest : public ::testing::Test {
 protected:
  NginxConfigParser parser;
  NginxConfig config;

  Handler* reverseProxyHandler;
  HttpRequest req;
  unique_ptr<HttpResponse> res;

  virtual void SetUp() {
    parser.Parse("config_parser_test_input/reverse_proxy_handler_config", &config);
    shared_ptr<NginxConfig> blockConfig = config.statements_[1]->child_block_;
    reverseProxyHandler = ReverseProxyHandler::create(*blockConfig, config.getRootPath());
  }

  virtual void TearDown() {
    delete reverseProxyHandler;
  }
};

TEST_F(ReverseProxyHandlerTest, TestSimpleProxy) {
  string input;
  input += "GET /ucla HTTP/1.1\r\n";
  input += "Host: localhost\r\n";
  input += "User-Agent: curl/7.58.0\r\n";
  input += "Accept: */*\r\n";
  input += "\r\n";

  req.parse(input);
  res = reverseProxyHandler->handle_request(req);

  string responseStr = res->to_string();
  EXPECT_THAT(responseStr, StartsWith("HTTP/1.1 200 OK\r\n"));
  EXPECT_THAT(responseStr, HasSubstr("Content-Type: text/html"));
  EXPECT_THAT(responseStr, HasSubstr("Featured Stories"));
}

TEST_F(ReverseProxyHandlerTest, TestGetSubdirectoryProxy) {
  string input;
  input += "GET /ucla/about HTTP/1.1\r\n";
  input += "Host: localhost\r\n";
  input += "User-Agent: curl/7.58.0\r\n";
  input += "Accept: */*\r\n";
  input += "\r\n";

  req.parse(input);
  res = reverseProxyHandler->handle_request(req);

  string responseStr = res->to_string();
  EXPECT_THAT(responseStr, StartsWith("HTTP/1.1 200 OK\r\n"));
  EXPECT_THAT(responseStr, HasSubstr("Content-Type: text/html"));
  EXPECT_THAT(responseStr, HasSubstr("For nearly 100 years, ucla has been a pioneer, persevering through impossibility, turning the futile into the attainable."));
}



} //namespace
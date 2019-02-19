#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "status_handler.h"
#include "data_store.h"
#include "location.h"
#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"

using ::testing::StartsWith;
using ::testing::HasSubstr;
using ::testing::EndsWith;

namespace {
class StatusHandlerTest : public ::testing::Test {
  virtual void SetUp() {
    statusHandler = StatusHandler::create(NginxConfig(), "", &dataStore);
    LocationMap locationMap;
    LocationInfo* info = new LocationInfo();
    info->handlerType = "handler-type";
    locationMap["/test"] = info;
    dataStore.setLocationMap(locationMap);
  }
  virtual void TearDown() {
    delete statusHandler;
  }
 protected:
  Handler* statusHandler;
  DataStore dataStore;
  HttpRequest req;
  unique_ptr<HttpResponse> res;
};

TEST_F(StatusHandlerTest, CheckGoodRequestHandler) {
  string input;
  input += "GET / HTTP/1.1\r\n";
  input += "Host: localhost\r\n";
  input += "User-Agent: curl/7.58.0\r\n";
  input += "Accept: */*\r\n";
  input += "\r\n";

  req.parse(input);
  dataStore.recordRequest("/", 200);
  res = statusHandler->handle_request(req);

  string responseStr = res->to_string();
  string expectedStr;
  expectedStr += "Requests received:\n";
  expectedStr += "/\n";
  expectedStr += "\t200: 1\n";
  expectedStr += "\n";
  expectedStr += "Routes and handlers:\n";
  expectedStr += "/test: handled by handler-type\n";
  EXPECT_THAT(responseStr, StartsWith("HTTP/1.1 200 OK\r\n"));
  EXPECT_THAT(responseStr, HasSubstr("Content-Type: text/plain\r\n"));
  EXPECT_THAT(responseStr, EndsWith(expectedStr));
}
}  // namespace

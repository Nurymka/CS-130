#include "gtest/gtest.h"
#include "http_response.h"

namespace {
class HttpResponseTest: public ::testing::Test {
 protected:
  HttpResponse res;
};

TEST_F(HttpResponseTest, CheckValidParsing) {
    res.version = "HTTP/1.1";
    res.status_code = 200;
    res.headers.push_back("Host: localhost:8080");
    res.body = "TEST";

    string output;
    output += "HTTP/1.1 200 OK\r\n";
    output += "Host: localhost:8080\r\n";
    output += "Content-Length: 4\r\n";
    output += "\r\n";
    output += "TEST";

    EXPECT_EQ(res.to_string(), output);
}
}  // namespace

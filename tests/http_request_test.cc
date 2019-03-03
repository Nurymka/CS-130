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
    input += "Content-Length: 100\r\n";
    input += "Content-Type: application/x-www-form-urlencoded\r\n";
    input += "\r\n";
    input += "firstname=First+Name%26other&lastname=Last%2Bthis%3Danother";

    bool success = req.parse(input);
    EXPECT_TRUE(success);
    EXPECT_EQ(req.method, "POST");
    EXPECT_EQ(req.target, "/");
    EXPECT_EQ(req.version, "HTTP/1.1");
    EXPECT_EQ(req.headers.size(), 5);
    EXPECT_EQ(req.body, "firstname=First+Name%26other&lastname=Last%2Bthis%3Danother");
    EXPECT_EQ(req.data.size(), 2);
    EXPECT_EQ(req.data["firstname"], "First Name&other");
    EXPECT_EQ(req.data["lastname"], "Last+this=another");
    EXPECT_EQ(req.to_string(), input);
}

TEST_F(HttpRequestTest, InvalidStartLine) {
    string input;
    input += "POST/ HTTP/1.1\r\n";
    input += "Host: localhost:8080\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "Content-Length: 4\r\n";
    input += "Content-Type: application/x-www-form-urlencoded\r\n";
    input += "\r\n";
    input += "TEST";

    bool success = req.parse(input);
    EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidStartLine2) {
    string input;
    input += "POST/HTTP/1.1\r\n";
    input += "Host: localhost:8080\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "Content-Length: 4\r\n";
    input += "Content-Type: application/x-www-form-urlencoded\r\n";
    input += "\r\n";
    input += "TEST";

    bool success = req.parse(input);
    EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidHeader) {
    string input;
    input += "POST / HTTP/1.1\r\n";
    input += "Host:localhost:8080\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "Content-Length: 4\r\n";
    input += "Content-Type: application/x-www-form-urlencoded\r\n";
    input += "\r\n";
    input += "TEST";

    bool success = req.parse(input);
    EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidHeader2) {
    string input;
    input += "POST/ HTTP/1.1\r\n";
    input += "Host:localhost:8080\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept:*/*\r\n";
    input += "Content-Length: 4\r\n";
    input += "Content-Type: application/x-www-form-urlencoded\r\n";
    input += "\r\n";
    input += "TEST";

    bool success = req.parse(input);
    EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, CheckBodyLength) {
    string input;
    input += "POST / HTTP/1.1\r\n";
    input += "Host: localhost:8080\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "Content-Length: 4\r\n";
    input += "Content-Type: application/x-www-form-urlencoded\r\n";
    input += "\r\n";
    input += "TEST";

    req.parse(input);
    EXPECT_EQ(req.body.length(), 4);
}

TEST_F(HttpRequestTest, EmptyBody) {
    string input;
    input += "GET / HTTP/1.1\r\n";
    input += "Host: localhost:8080\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "\r\n";

    bool success = req.parse(input);
    EXPECT_TRUE(success);
}

TEST_F(HttpRequestTest, FalseMethod) {
    string input;
    input += "POS / HTTP/1.1\r\n";
    input += "Host: localhost:8080\r\n";
    input += "User-Agent: curl/7.58.0\r\n";
    input += "Accept: */*\r\n";
    input += "Content-Length: 0\r\n";
    input += "Content-Type: application/x-www-form-urlencoded\r\n";
    input += "\r\n";

    bool success = req.parse(input);
    EXPECT_FALSE(success);
}
}  // namespace

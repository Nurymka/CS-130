#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "view_meme_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"
#include "meme_db.h"
#include "meme.h"
#include <vector>
#include <sstream>

namespace {
class ViewMemeHandlerTest: public ::testing::Test {
  protected:
  	ViewMemeHandler* viewMemeHandler;
    HttpRequest req;
    unique_ptr<HttpResponse> res;
    MemeDB* meme_db;
    Meme one = Meme("/dank/memes", "Hey guys!  Did you know that", "BOTTOM TEXT", "1");
    Meme two = Meme("/danker/memes", "That feeling when", "You write unit tests with outdated memes.", "2");

    virtual void SetUp() {
      viewMemeHandler = (ViewMemeHandler *) ViewMemeHandler::create(NginxConfig(), "");
      meme_db = new MemeDB("test.db");
      meme_db->add("/dank/memes", "Hey guys!  Did you know that", "BOTTOM TEXT");
      meme_db->add("/danker/memes", "That feeling when", "You write unit tests with outdated memes.");
      viewMemeHandler->setMemeDB(meme_db);
    }

    virtual void TearDown() {
    	meme_db->clear();
    	delete meme_db;
    }

};

TEST_F(ViewMemeHandlerTest, TestValidMemeRetrieval) {
  string input_one = "GET /target?id=1 HTTP/1.1\r\n\r\n";
  string input_two = "GET /target?id=2 HTTP/1.1\r\n\r\n";

  req.parse(input_one);

  res = viewMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 200);
  EXPECT_EQ(res->body, one.toJSON());

  req.parse(input_two);
  res = viewMemeHandler->handle_request(req);
  EXPECT_EQ(res->status_code, 200);
  EXPECT_EQ(res->body, two.toJSON());

}

TEST_F(ViewMemeHandlerTest, TestNonexistentMemeRetrieval) {
  string input_a = "GET /target?id=275 HTTP/1.1\r\n\r\n";
  req.parse(input_a);
  res = viewMemeHandler->handle_request(req);
  EXPECT_EQ(res->status_code, 404);
}

TEST_F(ViewMemeHandlerTest, TestInvalidRequest){
  // This handler should return a 400 status message for any non-GET requests.
  // Keeping this in mind, I borrowed a valid POST request from http_request_test.cc.

  string input;
  input += "POST / HTTP/1.1\r\n";
  input += "Host: localhost:8080\r\n";
  input += "User-Agent: curl/7.58.0\r\n";
  input += "Accept: */*\r\n";
  input += "Content-Length: 100\r\n";
  input += "Content-Type: application/x-www-form-urlencoded\r\n";
  input += "\r\n";
  input += "firstname=First+Name%26other&lastname=Last%2Bthis%3Danother";
  req.parse(input);
  res = viewMemeHandler->handle_request(req);
  EXPECT_EQ(res->status_code, 400);
}

}
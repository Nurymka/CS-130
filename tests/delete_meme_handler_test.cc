#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "delete_meme_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"
#include "meme_db.h"
#include "meme.h"
#include <vector>
#include <sstream>
#include <iostream>

namespace {
class DeleteMemeHandlerTest: public ::testing::Test {
  protected:
  	DeleteMemeHandler* deleteMemeHandler;
    HttpRequest req;
    unique_ptr<HttpResponse> res;
    MemeDB* meme_db;
    Meme one = Meme("/dank/memes", "Hey guys!  Did you know that", "BOTTOM TEXT", "1");
    Meme two = Meme("/danker/memes", "That feeling when", "You write unit tests with outdated memes.", "2");

    virtual void SetUp() {
      deleteMemeHandler = (DeleteMemeHandler *) DeleteMemeHandler::create(NginxConfig(), "");
      meme_db = new MemeDB("test_delete.db");
      meme_db->add("/dank/memes", "Hey guys!  Did you know that", "BOTTOM TEXT");
      meme_db->add("/danker/memes", "That feeling when", "You write unit tests with outdated memes.");
      deleteMemeHandler->setMemeDB(meme_db);
    }

    virtual void TearDown() {
    	meme_db->clear();
    	delete meme_db;
    }

};

TEST_F(DeleteMemeHandlerTest, TestValidDeletion) {
  string input;
  input += "POST / HTTP/1.1\r\n";
  input += "Host: localhost:8080\r\n";
  input += "User-Agent: curl/7.58.0\r\n";
  input += "Accept: */*\r\n";
  input += "Content-Length: 100\r\n";
  input += "Content-Type: application/x-www-form-urlencoded\r\n";
  input += "\r\n";
  input += "id=1";
  vector<Meme> memes_before_delete = meme_db->findAll();
  EXPECT_EQ(2, memes_before_delete.size());

  req.parse(input);
  res = deleteMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 200);

  vector<Meme> memes_after_delete = meme_db->findAll();
  EXPECT_EQ(1, memes_after_delete.size());

}

TEST_F(DeleteMemeHandlerTest, TestDoubleDeletion) {string input;
  input += "POST / HTTP/1.1\r\n";
  input += "Host: localhost:8080\r\n";
  input += "User-Agent: curl/7.58.0\r\n";
  input += "Accept: */*\r\n";
  input += "Content-Length: 100\r\n";
  input += "Content-Type: application/x-www-form-urlencoded\r\n";
  input += "\r\n";
  input += "id=1";string input_one = "POST /target?name=delete_1 HTTP/1.1\r\n\r\nid=1";
  vector<Meme> memes_before_delete = meme_db->findAll();
  EXPECT_EQ(2, memes_before_delete.size());

  req.parse(input);
  res = deleteMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 200);

  res = deleteMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 200);

  vector<Meme> memes_after_delete = meme_db->findAll();
  EXPECT_EQ(1, memes_after_delete.size());
}

TEST_F(DeleteMemeHandlerTest, TestDeletionInvalidID) {
  string input_one = "POST /target?name=delete_1 HTTP/1.1\r\n\r\nid=caribou";

  req.parse(input_one);
  res = deleteMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 400);

}

}
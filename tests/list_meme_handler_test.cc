#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "list_meme_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"
#include "meme_db.h"
#include "meme.h"
#include <vector>
#include <sstream>

namespace {
class ListMemeHandlerTest: public ::testing::Test {
  protected:
  	ListMemeHandler* listMemeHandler;
    HttpRequest req;
    unique_ptr<HttpResponse> res;
    MemeDB* meme_db;
    Meme one = Meme("/dank/memes", "Hey guys!  Did you know that", "BOTTOM TEXT", "1");
    Meme two = Meme("/danker/memes", "That feeling when", "You write unit tests with outdated memes.", "2");
    Meme three =  Meme("/danker/memes", "Hey guys!", "You don't want to come up a new meme.", "3");

    virtual void SetUp() {
      listMemeHandler = (ListMemeHandler *) ListMemeHandler::create(NginxConfig(), "");
      meme_db = new MemeDB("test_list.db");
      meme_db->add("/dank/memes", "Hey guys!  Did you know that", "BOTTOM TEXT");
      meme_db->add("/danker/memes", "That feeling when", "You write unit tests with outdated memes.");
      meme_db->add("/danker/memes", "Hey guys!", "You don't want to come up a new meme.");
      listMemeHandler->setMemeDB(meme_db);
    }

    virtual void TearDown() {
    	meme_db->clear();
    	delete meme_db;
    }

};

TEST_F(ListMemeHandlerTest, FullListRetrieval) {
  string input_str = "GET /target HTTP/1.1\r\n\r\n";
  req.parse(input_str);

  string expected_output;
  expected_output += "{ \"memes\" : [ ";
  expected_output += three.toJSON();
  expected_output += ", ";
  expected_output += two.toJSON();
  expected_output += ", ";
  expected_output += one.toJSON();
  expected_output += " ] }";


  res = listMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 200);
  EXPECT_EQ(res->body, expected_output);

}

TEST_F(ListMemeHandlerTest, SearchByPath) {
  string input_str = "GET /target?q=/danker/memes HTTP/1.1\r\n\r\n";
  req.parse(input_str);

  string expected_output;
  expected_output += "{ \"memes\" : [ ";
  expected_output += three.toJSON();
  expected_output += ", ";
  expected_output += two.toJSON();
  expected_output += " ] }";

  res = listMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 200);
  EXPECT_EQ(res->body, expected_output);
}


TEST_F(ListMemeHandlerTest, SearchByTop) {
  string input_str = "GET /target?q=Hey HTTP/1.1\r\n\r\n";
  req.parse(input_str);

  string expected_output;
  expected_output += "{ \"memes\" : [ ";
  expected_output += three.toJSON();
  expected_output += ", ";
  expected_output += one.toJSON();
  expected_output += " ] }";

  res = listMemeHandler->handle_request(req);  
  EXPECT_EQ(res->status_code, 200);
  EXPECT_EQ(res->body, expected_output);
}
}
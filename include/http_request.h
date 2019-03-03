#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unordered_map>

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

using namespace std;


// implements http_request class
// parts of the codes adapted or inspired from following urls: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// https://stackoverflow.com/questions/10219225/c-create-string-of-text-and-variables
// http://www.cplusplus.com/reference/string/string/substr/
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_parser.cpp


class HttpRequest {
 public:
    // 3 elements in start line;
    string method;
    string target;
    string version;

    // headers: content type need to be set to “text/plain”
    vector<string> headers;

    // messageBody:
    string body;
    unordered_map <string, string> data;

    int contentLength;

    bool parse(const string& raw_message);
    string to_string() const;

 private:
    vector<string> split_str(const string& s, string c);
    string clean_str(string s);
    string decode_url(string s);

    enum ParseState {
      START_LINE = 0,
      HEADERS = 1,
      BODY = 2,
    };
};

#endif  // HTTP_REQUEST_H_

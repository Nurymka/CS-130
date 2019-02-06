#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>

#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

using namespace std;

// part of the codes adapted or inspired from following urls:
// https://stackoverflow.com/questions/2417588/escaping-a-c-string
// https://en.cppreference.com/w/cpp/string/basic_string/replace
// http://www.cplusplus.com/forum/beginner/71782/
// https://github.com/curl/curl/issues/232

class HttpResponse {
 public:
    string version;
    int status_code;
    vector<string> headers;
    string body;

    string to_string();

    static const string kBadRequestMessage;
 private:
    string escape(string input) {
      return input;
    }
};

#endif  // HTTP_RESPONSE_H_

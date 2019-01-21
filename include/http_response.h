#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#ifndef HttpResponse_H
#define HttpResponse_H

using namespace std;

// part of the codes adapted or inspired from following urls:
//https://stackoverflow.com/questions/2417588/escaping-a-c-string
//https://en.cppreference.com/w/cpp/string/basic_string/replace
//http://www.cplusplus.com/forum/beginner/71782/
//https://github.com/curl/curl/issues/232

class HttpResponse {
  public:
    string version;
    int status_code;
    vector<string> headers; 
    string body;

    string to_string() {
      string escaped = escape(body);

      string status;
      switch(status_code) {
        case 200:
          status = "OK";
          break;
      }

      std::ostringstream oss;
      oss << version.c_str() << " " << std::to_string(status_code) << " " << status << "\r\n";
      for(auto const &value: headers) {
        oss << value << "\r\n";
      }
      int contentLength = escaped.size() + 2 ;
      oss << "Content-Length: " << contentLength << "\r\n";
      oss << "\r\n";
      oss << escaped;
      oss << "\r\n";
      return oss.str();
    }
  private:
    string escape(string input) {
      
      return input;
    }
};

#endif

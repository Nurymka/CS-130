#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "http_response.h"

using namespace std;

// part of the codes adapted or inspired from following urls:
//https://stackoverflow.com/questions/2417588/escaping-a-c-string
//https://en.cppreference.com/w/cpp/string/basic_string/replace
//http://www.cplusplus.com/forum/beginner/71782/
//https://github.com/curl/curl/issues/232
//https://stackoverflow.com/questions/2528995/remove-r-from-a-string-in-c

string HttpResponse::to_string() {
    string escaped = escape(body);

    string status;
    switch(status_code) {
    case 200:
        status = "OK";
        break;
    case 400:
        status = "Bad Request";
        break;
    }

    std::ostringstream oss;
    oss << version.c_str() << " " << std::to_string(status_code) << " " << status << "\r\n";
    for(auto const &value: headers) {
    oss << value << "\r\n";
    }
    int contentLength = escaped.size();
    oss << "Content-Length: " << contentLength << "\r\n";
    oss << "\r\n";
    oss << escaped;
    return oss.str();
}

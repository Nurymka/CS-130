#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "http_request.h"

using namespace std;


// implements http_request class
// parts of the codes adapted or inspired from following urls: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// https://stackoverflow.com/questions/10219225/c-create-string-of-text-and-variables
// http://www.cplusplus.com/reference/string/string/substr/
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_parser.cpp


bool HttpRequest::parse(const string& data) {
    std::istringstream input(data);
    string line;

    ParseState state = START_LINE;
    while(getline(input, line)) {
      line = clean_str(line);
      if (state == START_LINE) {
          vector<string> tokens = split_str(line, " ");
          if(tokens.size() != 3) {
            return false;
          }
          method = tokens[0];
          if(method.compare("GET") != 0 && method.compare("PUT") != 0 
            && method.compare("PATCH") != 0 && method.compare("DELETE") != 0
              && method.compare("POST") != 0) {
            return false;
          }
          target = tokens[1];
          version = tokens[2];
          state = HEADERS;
      } else if (state == HEADERS) {
          if (line.empty() || line.compare("\n") == 0 || line.compare("\r\n") ==0 || line.compare("\r") == 0) {
            state = BODY;
          } else {
            vector<string> headerStrs = split_str(line, ":");
            // header should be of format 'NAME: VALUE'
            if (headerStrs.size() < 1 || headerStrs[1].at(0) != ' ') {
              return false;
            }
            if(headerStrs[0].compare("Content-Length") == 0) {
              try {
                string len = headerStrs[1].substr(1, headerStrs[1].length());
                this->contentLength = atoi(len.c_str());
              } catch (exception const & e) {
                return false;
              }
            }
            headers.push_back(line);
          }
      } else {
          if (!body.empty()) {
          body.append("\r\n");
          }
          body.append(line);
      }
    }
    if(body.length() != contentLength) {
      return false;
    }
    return true;
}

string HttpRequest::to_string() {
    std::ostringstream oss;
    oss << method << " " << target << " " << version << "\r\n";
    for(auto const &header: headers) {
    oss << header << "\r\n";
    }
    oss << "\r\n";
    oss << body;
    return oss.str();
}

vector<string> HttpRequest::split_str(const string& s, string c) {
    vector<string> v;
    boost::algorithm::split(v, s, boost::is_any_of(c));
    return v;
}

string HttpRequest::clean_str(string s) {
  if (!s.empty() && s[s.size() - 1] == '\n')
    s.erase(s.size() - 1);
  if (!s.empty() && s[s.size() - 1] == '\r') {
    s.erase(s.size() - 1);
  }
  return s;
}

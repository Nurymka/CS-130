#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include "http_request.h"

using namespace std;


// implements http_request class
// parts of the codes adapted or inspired from following urls: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// https://stackoverflow.com/questions/10219225/c-create-string-of-text-and-variables
// http://www.cplusplus.com/reference/string/string/substr/
// https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_parser.cpp


bool HttpRequest::parse(const string& raw_message) {
    std::istringstream input(raw_message);
    string line;

    // Not assigning directly to the instance methods to preserve atomicity during parsing
    string _method;
    string _target;
    string _version;
    vector<string> _headers;
    string _body;
    unordered_map<string, string> _data;
    int _contentLength = 0;

    ParseState state = START_LINE;

    while (getline(input, line)) {
      line.append("\n");  // add back new-line
      string cleanLine = clean_str(line);
      if (state == START_LINE) {
          vector<string> tokens = split_str(cleanLine, " ");
          if (tokens.size() != 3) {
            return false;
          }
          _method = tokens[0];
          if (_method.compare("GET") != 0 && _method.compare("PUT") != 0
            && _method.compare("PATCH") != 0 && _method.compare("DELETE") != 0
              && _method.compare("POST") != 0) {
            return false;
          }
          _target = tokens[1];
          _version = tokens[2];
          if (_version.compare("HTTP/1.1") != 0)
            return false;
          state = HEADERS;
      } else if (state == HEADERS) {
          if (line.compare("\r\n") == 0) {
            state = BODY;
            break;
          } else {
            vector<string> headerStrs = split_str(cleanLine, ":");
            // header should be of format 'NAME: VALUE'
            if (headerStrs.size() < 1 || headerStrs[1].at(0) != ' ') {
              return false;
            }
            if (headerStrs[0].compare("Content-Length") == 0) {
              try {
                string len = headerStrs[1].substr(1, headerStrs[1].length());
                _contentLength = atoi(len.c_str());
              } catch (exception const & e) {
                return false;
              }
            }
            _headers.push_back(cleanLine);
          }
      }
    }
    if (state == BODY) {
      int bodyRead = 0;
      char c;
      while (bodyRead < _contentLength && input.get(c)) {
        _body += c;
        bodyRead++;
      }
    }

    // Parse POST data.
    if (_method == "POST") {
      vector<string> attrs = split_str(_body, "&");
      for (auto const &attr : attrs) {
        vector<string> name_value = split_str(attr, "=");
        if (name_value.size() != 2) {
          continue;
        }
        string name = decode_url(name_value[0]);
        string value = decode_url(name_value[1]);
        _data[name] = value;
      }
    }

    method = _method;
    target = _target;
    version = _version;
    headers = _headers;
    body = _body;
    data = _data;
    contentLength = _contentLength;

    return true;
}

string HttpRequest::to_string() const {
    std::ostringstream oss;
    oss << method << " " << target << " " << version << "\r\n";
    for (auto const &header : headers) {
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

// adapted from https://stackoverflow.com/questions/2673207/c-c-url-decode-library
string HttpRequest::decode_url(string s) {
  const char* src = s.c_str();
  char* dst = new char[s.length()+1];
  char* decoded = dst;  // save pointer to beginning of string to return at the end
  char a, b;
  while (*src) {
    if ((*src == '%') &&
        ((a = src[1]) && (b = src[2])) &&
        (isxdigit(a) && isxdigit(b))) {
          // decode char (e.g. "%26" decoded is '&')
          if (a >= 'a')
            a -= 'a'-'A';
          if (a >= 'A')
            a -= ('A' - 10);
          else
            a -= '0';
          if (b >= 'a')
            b -= 'a'-'A';
          if (b >= 'A')
            b -= ('A' - 10);
          else
            b -= '0';
          *dst++ = 16*a+b;
          src+=3;
      } else if (*src == '+') {
        // replace with space
        *dst++ = ' ';
        src++;
      } else {
        // simply copy over char
        *dst++ = *src++;
      }
  }
  *dst++ = '\0';  // end c-string with null-byte
  return string(decoded);
}

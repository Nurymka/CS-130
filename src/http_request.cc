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

    // Not assigning directly to the instance methods to preserve atomicity during parsing
    string _method;
    string _target;
    string _version;
    vector<string> _headers;
    string _body;
    int _contentLength = 0;

    ParseState state = START_LINE;

    while (getline(input, line)) {
      line.append("\n");  // add back new-line
      string cleanLine = clean_str(line);
      if (state == START_LINE) {
          vector<string> tokens = split_str(cleanLine, " ");
          if (tokens.size() != 3) {
            cout << "==========T" << "\\" + line << endl;
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
      // if body length is less than specified content-length should this be error?
      // Example, could be seen as data loss/incomplete transfer.
      // body length greater than specified content-length is NOT treated as error
      // (simply ignore any message content greater than specified content-length)
      if (_body.length() != _contentLength) {
        return false;
      }
    }

    method = _method;
    target = _target;
    version = _version;
    headers = _headers;
    body = _body;
    contentLength = _contentLength;

    return true;
}

string HttpRequest::to_string() {
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

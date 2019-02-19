// An nginx config file parser.

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <memory>
#include <stack>
#include <cstring>
#include <unordered_map>
#include <map>
#include "handler.h"

#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_
using namespace std;

class NginxConfig;

// The parsed representation of a single config statement.
class NginxConfigStatement {
 public:
  string ToString(int depth);
  vector<string> tokens_;
  shared_ptr<NginxConfig> child_block_;
};

// The parsed representation of the entire config.
class NginxConfig {
 public:
  string ToString(int depth = 0);
  vector<shared_ptr<NginxConfigStatement>> statements_;
  int16_t getPort();

  // Returns root path without trailing '/' character at the end.
  string getRootPath();

  // Returns a dictionary of supported locations (e.g. "/echo", "/static1")
  // with their respective config
  // TODO(nurymka): change LocationInfo* into a unique_ptr, had problems
  // with compilation when I tried. The owner of the map should be the server.
  // For now, it'll be the one reponsible for deallocing LocationInfo objects.
  LocationMap getLocationInfos();
  unordered_map<string, HandlerMaker*> getTargetToHandler();
 private:
  // Gets value for a top-level keyword (e.g. 'root', 'port')
  // True if statement exists, T& value is set accordingly; false otherwise.
  template<typename T> bool getTopLevelStatement(const string& keyword, T& value);
};

// The driver that parses a config file and generates an NginxConfig.
class NginxConfigParser {
 public:
  NginxConfigParser() {}

  // Take a opened config file or file name (respectively) and store the
  // parsed config in the provided NginxConfig out-param.  Returns true
  // iff the input config file is valid.
  bool Parse(istream* config_file, NginxConfig* config);
  bool Parse(const char* file_name, NginxConfig* config);

 private:
  enum TokenType {
    TOKEN_TYPE_START = 0,
    TOKEN_TYPE_NORMAL = 1,
    TOKEN_TYPE_START_BLOCK = 2,
    TOKEN_TYPE_END_BLOCK = 3,
    TOKEN_TYPE_COMMENT = 4,
    TOKEN_TYPE_STATEMENT_END = 5,
    TOKEN_TYPE_EOF = 6,
    TOKEN_TYPE_ERROR = 7
  };
  const char* TokenTypeAsString(TokenType type);

  enum TokenParserState {
    TOKEN_STATE_INITIAL_WHITESPACE = 0,
    TOKEN_STATE_SINGLE_QUOTE = 1,
    TOKEN_STATE_DOUBLE_QUOTE = 2,
    TOKEN_STATE_TOKEN_TYPE_COMMENT = 3,
    TOKEN_STATE_TOKEN_TYPE_NORMAL = 4
  };

  TokenType ParseToken(istream* input, string* value);
};

#endif  // CONFIG_PARSER_H_

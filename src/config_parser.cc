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

#include "config_parser.h"
#include "handler.h"
#include "echo_handler.h"
#include "static_handler.h"

using namespace std;

string NginxConfig::ToString(int depth) {
  string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

template<typename T>
bool NginxConfig::getTopLevelStatement(const string& keyword, T& value) const {
  for (const auto& statement : statements_) {
    string cur_keyword = statement->tokens_[0];
    if (keyword.compare(cur_keyword) == 0) {
      stringstream valueString(statement->tokens_[1]);
      valueString >> value;
      return true;
    }
  }
  return false;
}

// Added a function that searches for portNumber in statements_
int16_t NginxConfig::getPort() {
  int16_t port = -1;
  getTopLevelStatement<int16_t>("port", port);
  return port;
}

string NginxConfig::getRootPath() {
  string rootPath;
  getTopLevelStatement<string>("root", rootPath);
  if (rootPath[rootPath.length() - 1] == '/')
    rootPath.erase(rootPath.end() - 1, rootPath.end());
  return rootPath;
}

LocationMap NginxConfig::getLocationInfos() {
  LocationMap locationInfos;

  for (const auto& statement : statements_) {
    vector<string> tokens = statement->tokens_;
    // Checks if the current token is a handler
    if (tokens.size() == 2 && tokens[0] == "handler") {
      string location;
      LocationInfo *info = new LocationInfo();
      info->handlerType = tokens[1];
      info->blockConfig = statement->child_block_;

      // In the handler block, looks for a 'location' statement
      for (const auto& blockStatement : info->blockConfig->statements_) {
        vector<string> blockTokens = blockStatement->tokens_;
        if (blockTokens[0] == "location") {
          location = blockTokens[1];
          break;
        }
      }

      // map.find(location) == map.end() implies that in case of duplicate
      // hanlders in same location, the first one in config takes priority.
      if (!location.empty() && locationInfos.find(location) == locationInfos.end()) {
        locationInfos[location] = info;
      } else {
        delete info;
      }
    }
  }

  return locationInfos;
}

unordered_map<string, HandlerMaker*> NginxConfig::getTargetToHandler() {
  unordered_map<string, HandlerMaker*> targetToHandler;

  for (const auto& statement : statements_) {
    vector<string> tokens = statement->tokens_;
    if (tokens[0] == "http" || tokens[0] == "server") {
      NginxConfig childConfig = *(statement->child_block_);
      unordered_map<string, HandlerMaker*> childMap = childConfig.getTargetToHandler();
      if (childMap.size() > 0) {
        return childMap;
      }
    } else if (tokens.size() > 1 && tokens[0] == "location") {
      string target = tokens[1];
      string type;

      for (const auto& childStatement : statement->child_block_->statements_) {
        vector<string> childTokens = childStatement->tokens_;
        if (childTokens[0] == "echo") {
          targetToHandler[target] = new EchoHandlerMaker();
        }
        if (childTokens.size() > 1 && (childTokens[0] == "alias" || childTokens[0] == "root")) {
          // TODO(nate): add static file handler here
          targetToHandler[target] = new StaticHandlerMaker(childTokens[1], target);
        }
      }
    }
  }
  return targetToHandler;
}

string NginxConfigStatement::ToString(int depth) {
  string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(istream* input,
                                                           string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        // TODO(xiangyin701): the end of a quoted token should be followed by whitespace.
        // TODO(xiangyin701): Maybe also define a QUOTED_STRING token type.
        // TODO(xiangyin701): Allow for backslash-escaping within strings.
        *value += c;
        if (c == '\'') {
          return TOKEN_TYPE_NORMAL;
        } else if (c == '\\') {  // added additional conditions where ['abd\';] should be illegal
          if (input->good()) {
            *value += input->get();
          }
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          return TOKEN_TYPE_NORMAL;
        } else if (c == '\\') {  // added additional conditions where  ["abd\";] should be illegal
          if (input->good()) {
            *value += input->get();
          }
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state != TOKEN_STATE_INITIAL_WHITESPACE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(istream* config_file, NginxConfig* config) {
  stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  int start_blocks = 0;

  while (true) {
    string token;
    token_type = ParseToken(config_file, &token);

    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
     start_blocks++;
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if ((last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK &&
          last_token_type != TOKEN_TYPE_START_BLOCK) ||
          start_blocks <= 0) {
        // Error.
        break;
      }
      start_blocks--;
      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if ((last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK) ||
          start_blocks != 0) {
        // Error.
        break;
      }
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  // printf ("Bad transition from %s to %s\n",
  //         TokenTypeAsString(last_token_type),
  //         TokenTypeAsString(token_type));
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf("Failed to open config file: %s\n", file_name);
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<istream*>(&config_file), config);
  config_file.close();
  return return_value;
}


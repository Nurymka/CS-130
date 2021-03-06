#include <string>
#include "location.h"

using namespace std;

LocationMap LocationUtils::getLocationMapFrom(const NginxConfig& config) {
  LocationMap locationMap;

  for (const auto& statement : config.statements_) {
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
      if (!location.empty() && locationMap.find(location) == locationMap.end()) {
        locationMap[location] = info;
      } else {
        delete info;
      }
    }
  }

  return locationMap;
}

LocationInfo* LocationUtils::getLongestMatchingLocation(const string& reqLocation,
  const LocationMap& locMap) {
  string str = reqLocation;

  while (str.length() > 0) {
    if (locMap.find(str) != locMap.end()) {
     return locMap.at(str);
    }
    size_t index = str.rfind('/');
    if (index != string::npos) {
      str.erase(index);
    }
  }
  return nullptr;
}

string LocationUtils::extractPathOnly(const string& location) {
  string res = location;

  size_t queryIdx = res.find('?');
  if (queryIdx != string::npos)
    res.erase(queryIdx);

  if (res.back() == '/')
    res.erase(res.end() - 1, res.end());

  return res;
}

string LocationUtils::concatPaths(string fPath, string sPath) {
  if (fPath.at(fPath.length() - 1) == '/')
    fPath.erase(fPath.end() - 1, fPath.end());

  if (sPath.at(0) == '/')
    sPath = sPath.substr(1);

  return fPath + '/' + sPath;
}

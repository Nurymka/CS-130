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
  for (auto& locMapEntry : locMap) {
    string regLocation = locMapEntry.first;
    int regLocationDepth = LocationUtils::getLocationDepth(regLocation);

    if (regLocationDepth == -1)
      return nullptr;

    string stripReqLocation = LocationUtils::extractPathWithDepth(reqLocation, regLocationDepth);
    if (regLocation == stripReqLocation)
      return locMapEntry.second;
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

int LocationUtils::getLocationDepth(const string& location) {
  if (location == "/")
    return 0;

  int depth = 0;
  string path = LocationUtils::extractPathOnly(location);
  for (int i = 0; i < path.length(); i++) {
    if (path[i] == '/')
      depth++;
  }

  if (depth == 0)
    return -1;

  return depth;
}

string LocationUtils::extractPathWithDepth(const string& location, size_t depth) {
  if (depth == 0)
    return "/";

  string path = LocationUtils::extractPathOnly(location);
  string res;
  int slashOccurs = 0;
  for (int i = 0; i < path.length(); i++) {
    if (path[i] == '/')
      slashOccurs++;
    if (slashOccurs > depth)
      break;
    res += path[i];
  }

  return res;
}

string LocationUtils::concatPaths(string fPath, string sPath) {
  if (fPath.at(fPath.length() - 1) == '/')
    fPath.erase(fPath.end() - 1, fPath.end());

  if (sPath.at(0) == '/')
    sPath = sPath.substr(1);

  return fPath + '/' + sPath;
}

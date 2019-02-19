#include "location.h"

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
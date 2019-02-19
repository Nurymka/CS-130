#include <map>
#include "config_parser.h"

#ifndef LOCATION_H_
#define LOCATION_H_

// A predicate that sorts keys in descending length
struct LocationComp {
  bool operator() (const string& lhs, const string& rhs) const {
    return lhs.length() > rhs.length();
  }
};

struct LocationInfo {
  string handlerType;
  shared_ptr<NginxConfig> blockConfig;
};

// TODO(nurymka): change LocationInfo* into a unique_ptr, had problems
// with compilation when I tried. The owner of the map should be the server.
// For now, it'll be the one reponsible for deallocing LocationInfo objects.
typedef map<string, LocationInfo*, LocationComp> LocationMap;

class LocationUtils {
 public:
  // Returns a dictionary of supported locations (e.g. "/echo", "/static1")
  // with their respective config.
  static LocationMap getLocationMapFrom(const NginxConfig& config);
};

#endif  // LOCATION_H_

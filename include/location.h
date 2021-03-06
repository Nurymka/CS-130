#include <map>
#include <string>
#include <memory>
#include "config_parser.h"

#ifndef LOCATION_H_
#define LOCATION_H_

using namespace std;

struct LocationInfo {
  string handlerType;
  shared_ptr<NginxConfig> blockConfig;
};

// TODO(nurymka): change LocationInfo* into a unique_ptr, had problems
// with compilation when I tried. The owner of the map should be the server.
// For now, it'll be the one reponsible for deallocing LocationInfo objects.
typedef map<string, LocationInfo*> LocationMap;

class LocationUtils {
 public:
  // Returns a dictionary of supported locations (e.g. "/echo", "/static1")
  // with their respective config.
  static LocationMap getLocationMapFrom(const NginxConfig& config);
  static LocationInfo* getLongestMatchingLocation(const string& reqLocation,
    const LocationMap& locMap);

  // Strips any components (e.g. query) and returns location path only.
  // Also, strips any last trailing '/'.
  // For example, "/static/file1/?tag=new" becomes "/static/file1"
  static string extractPathOnly(const string& location);

  // Returns the concatenated result of two paths.
  static string concatPaths(string fPath, string sPath);
};

#endif  // LOCATION_H_

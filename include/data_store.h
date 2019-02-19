#include <vector>
#include <map>
#include <string>
#include "location.h"

#ifndef DATA_STORE_H_
#define DATA_STORE_H_

using namespace std;

// https://stackoverflow.com/questions/1463707/c-singleton-vs-global-static-object
class DataStore {
 public:
  static DataStore& getDataStore();
  void recordRequest(string target, int status);
  const map<string, map<int, int>>& getRequestCounts();
  void setLocationMap(LocationMap& locationMap);
  const LocationMap& getLocationMap();

 private:
  map<string, map<int, int>> requestCounts_;
  LocationMap locationMap_;
};

#endif  // DATA_STORE_H_

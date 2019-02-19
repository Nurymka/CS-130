#include <vector>
#include <map>
#include <string>

#include "location.h"
#include "data_store.h"

DataStore & DataStore::getDataStore() {
  static DataStore dataStore;
  return dataStore;
}

void DataStore::recordRequest(string target, int status) {
  requestCounts_[target][status]++;
}


const map<string, map<int, int>>& DataStore::getRequestCounts() {
  return requestCounts_;
}

void DataStore::setLocationMap(LocationMap& locationMap) {
  locationMap_ = locationMap;
}
const LocationMap& DataStore::getLocationMap() {
  return locationMap_;
}

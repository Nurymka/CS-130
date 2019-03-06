#include <vector>
#include <map>
#include <string>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include "location.h"
#include "data_store.h"

// https://stackoverflow.com/questions/989795/example-for-boost-shared-mutex-multiple-reads-one-write
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2406.html#shared_lock_rationale
typedef boost::shared_lock<boost::shared_mutex> ReaderLock;
typedef boost::unique_lock<boost::shared_mutex> WriterLock;

DataStore & DataStore::getDataStore() {
  static DataStore dataStore;
  return dataStore;
}

void DataStore::recordRequest(string target, int status) {
  WriterLock lock(requestCountsMutex_);
  requestCounts_[target][status]++;
}

const map<string, map<int, int>>& DataStore::getRequestCounts() {
  ReaderLock lock(requestCountsMutex_);
  return requestCounts_;
}

void DataStore::setLocationMap(LocationMap& locationMap) {
  locationMap_ = locationMap;
}
const LocationMap& DataStore::getLocationMap() {
  return locationMap_;
}

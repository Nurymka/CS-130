#include <memory>
#include <map>
#include <sstream>
#include "http_request.h"
#include "http_response.h"
#include "status_handler.h"
#include "data_store.h"
#include "location.h"

using namespace std;

Handler* StatusHandler::create(const NginxConfig& config,
                             const string& root_path) {
  return new StatusHandler();
}

Handler* StatusHandler::create(const NginxConfig& config,
                             const string& root_path,
                             DataStore* dataStore) {
  return new StatusHandler(dataStore);
}

StatusHandler::StatusHandler() {
  dataStore_ = &DataStore::getDataStore();
}

StatusHandler::StatusHandler(DataStore* dataStore)
  : dataStore_(dataStore) {}

unique_ptr<HttpResponse> StatusHandler::handle_request(const HttpRequest& req) {
  ostringstream oss;
  map<string, map<int, int>> requestCounts = dataStore_->getRequestCounts();
  oss << "Requests received:" << endl;

  for (auto const& request : requestCounts) {
    oss << request.first << endl;
    for (auto const& statusCount : request.second) {
      oss << "\t" << statusCount.first <<  ": " << statusCount.second << endl;
    }
  }

  LocationMap locationMap = dataStore_->getLocationMap();

  oss << endl << "Routes and handlers:" << endl;
  for (auto const& location : locationMap) {
    oss << location.first << ": handled by " << location.second->handlerType << endl;
  }

  unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;
  res->status_code = 200;
  res->headers.push_back("Content-Type: text/plain");
  res->body = oss.str();
  return res;
}

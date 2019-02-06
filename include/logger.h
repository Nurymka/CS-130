#include <cstdlib>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>

#ifndef LOGGER_H_
#define LOGGER_H_

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

enum severity_level {
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

typedef src::severity_logger<severity_level> severity_logger;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(server_log, severity_logger);

class Logger {
 public:
  static severity_logger& get();

 private:
  static severity_logger* instance_;
};

#endif  // LOGGER_H_

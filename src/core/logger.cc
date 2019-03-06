#include "logger.h"

// Taken from example code on
// https://stackoverflow.com/questions/15853981/boost-log-2-0-empty-severity-level-in-logs
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl) {
    static const char* const str[] = {
      "INFO",
      "WARNING",
      "ERROR",
      "CRITICAL"
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}

severity_logger* Logger::instance_ = nullptr;

severity_logger& Logger::get() {
  if (instance_ == nullptr) {
    logging::register_simple_formatter_factory<severity_level, char>("Severity");
    logging::add_file_log(
      keywords::file_name = "server_%Y%m%d.log",
      keywords::format = "%TimeStamp%: Thread = %ThreadID%, <%Severity%>: %Message%",
      keywords::rotation_size = 10 * 1024 * 1024,
      keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0));
    logging::add_console_log(
      std::cout,
      keywords::format = "%TimeStamp%: Thread = %ThreadID%, <%Severity%>: %Message%");
    logging::add_common_attributes();
    instance_ = &server_log::get();
  }

  return *instance_;
}

/** @file logger.h
 *  @brief The define logger class
 *  @author Bobrov A.E.
 *  @date 12.09.2016
 */
#ifndef GEOCODER_UTILS_LOGGER_LOGGER_H_
#define GEOCODER_UTILS_LOGGER_LOGGER_H_

// Boost
// logger
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
// filesystem
#include <boost/filesystem.hpp>

namespace geocoder
{
namespace utils
{
namespace logger
{
/** @enum Severity 
 *  @brief severity level logging
 */
enum class Severity
{
  info = 0,
  warning,
  error,
  crititcal,
  fatal,
  debug,
  trace
};

using SeverityLogger = boost::log::sources::severity_logger_mt<Severity>;

class Logger final
{
public:
 static void init();
 static void initFromFile(const boost::filesystem::path &filename);
};
}
}
}

BOOST_LOG_GLOBAL_LOGGER(geo_logger, geocoder::utils::logger::SeverityLogger)


    



#endif

/** @file log_config.h
 * @brief the logger configuration
 * @author Bobrov A.E.
 * @date 09.07.2016
 */
#ifndef GEOCODER_COMMON_LOGGER_CONFIG_H_
#define GEOCODER_COMMON_LOGGER_CONFIG_H_

// std
#include <cstdint>
#include <map>
#include <string>

// boost
#include <boost/filesystem.hpp>

namespace geocoder
{
namespace utils
{
namespace logger
{
namespace config
{
/** @struct Configuration
 * @brief The configuration logger
 */
struct Configuration
{
  using Attributes = std::map<std::string, bool>;

  struct Rotation
  {
    enum class Type
    {
      time,
      size
    };

    Type type{Type::time};
    std::uint32_t period{3600};
    std::uint64_t size{10};
  };

  enum class Time
  {
    utc,
    local
  };

  struct AttributesValues
  {
    static const std::string process_id;
    static const std::string thread_id;
    static const std::string timestamp;
  };

  bool stdoutput{true};
  boost::filesystem::path workdir = boost::filesystem::current_path();
  std::string filename = "geocoder_%Y-%m-%d_%H-%M-%S.%N.log";
  Rotation rotation;
  Time time_type{Time::utc};
  Attributes attributes{{AttributesValues::process_id, true}, {AttributesValues::thread_id, true}, {AttributesValues::timestamp, true}};
};

/** @brief read configuration from file */
Configuration readFile(const boost::filesystem::path &filename);
}  // namespace config
}  // namespace logger
}  // namespace utils
}  // namespace geocoder

#endif

/** @file config.cpp
 * @brief the implementation config
 * @author Bobrov A.E.
 * @date 09.07.2016
 */
// boost
// property tree
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
// string
#include <boost/algorithm/string.hpp>

// std
#include <stdexcept>

// this
#include "utils/logger/config.h"

namespace geocoder
{
namespace utils
{
namespace logger
{
namespace config
{
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
//--------------------------------------------------------------------------------------------
const std::string Configuration::AttributesValues::process_id = "ProcessID";
const std::string Configuration::AttributesValues::thread_id = "ThreadID";
const std::string Configuration::AttributesValues::timestamp = "TimeStamp";
//---------------------------------------------------------------------------------------------------------
Configuration readFile(const boost::filesystem::path &filename)
{
  if (!fs::exists(filename))
  {
    throw std::runtime_error("[logger::config::readFile]: is not exists file '" + filename.string() + "'");
  }

  pt::ptree document;

  pt::read_xml(filename.string(), document);
  Configuration conf;

  try
  {
    if (auto log_conf = document.get_child_optional("document.logger"))
    {
      conf.stdoutput = log_conf->get("stdout", false);
      conf.workdir = log_conf->get<fs::path>("workdir");
      conf.filename = log_conf->get<std::string>("filename");

      const auto time_type = log_conf->get<std::string>("time");

      if (time_type == "utc")
      {
        conf.time_type = Configuration::Time::utc;
      }
      else if (time_type == "local")
      {
        conf.time_type = Configuration::Time::local;
      }
      else
      {
        throw std::runtime_error("[logger::config::readFile]: invalid time type '" + time_type + "', filename ='" + filename.string() + "'");
      }

      if (const auto rotation = log_conf->get_child_optional("rotation"))
      {
        conf.rotation.period = rotation->get<std::uint32_t>("period");
        conf.rotation.size = rotation->get<std::uint64_t>("size");
        const auto type = rotation->get<std::string>("type");

        if (type == "time")
        {
          conf.rotation.type = Configuration::Rotation::Type::time;
        }
        else if (type == "size")
        {
          conf.rotation.type = Configuration::Rotation::Type::size;
        }
        else
        {
          throw std::runtime_error("[logger::config::readFile]: invalid rotation type '" + type + "', filename = '" + filename.string() + "'");
        }
      }

      for (auto &i : conf.attributes)
      {
        auto key(std::get<0>(i));
        boost::to_lower(key);
        const auto value = log_conf->get_optional<bool>("attributes." + key);
        if (value)
        {
          i.second = *value;
        }
      }

      return conf;
    }
    else
    {
      throw std::runtime_error("[logger::config::readFile]: is not found section 'logger'");
    }
  }
  catch (const std::runtime_error &)
  {
    throw;
  }
  catch (const std::exception &err)
  {
    throw std::runtime_error("[logger::config::readFile]: failed read file '" + std::string(err.what()) + "' filename = '" + filename.string() + "'");
  }
}
//---------------------------------------------------------------------------------------------------------
}  // namespace config
}  // namespace logger
}  // namespace utils
}  // namespace geocoder

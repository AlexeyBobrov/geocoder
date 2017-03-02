/** @file logger.cpp
 *  @brief The implementation logger
 *  @author Bobrov A.E.
 *  @date 12.09.2016
 */
// declare
#include "utils/logger/logger.h"

// std
#include <memory>
#include <array>
#include <string>
#include <functional>

// boost
//  logger
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>
//  memory
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
//  ptree
#include <boost/property_tree/ptree.hpp>
//  date_time
#include <boost/date_time/posix_time/posix_time.hpp>
//  utils
#include <boost/core/null_deleter.hpp>

// this
#include "utils/logger/config.h"
#include "utils/utils.h"

BOOST_LOG_GLOBAL_LOGGER_INIT(geo_logger, geocoder::utils::logger::SeverityLogger)
{
  geocoder::utils::logger::SeverityLogger lg;
  return lg;
}

namespace geocoder
{
namespace utils
{
namespace logger
{
//-------------------------------------------------------------------------------------------- 
namespace sinks = boost::log::sinks;
namespace sources = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace log = boost::log;
//-------------------------------------------------------------------------------------------- 
std::array<std::string, static_cast<int>(Severity::trace) + 1> text_sev
{
  "INFO", "WARNING", "ERROR", "CRITICAL", "FATAL", "DEBUG", "TRACE"
};
//-------------------------------------------------------------------------------------------- 
using sev_log_t = sources::severity_logger_mt<Severity>;
using file_sink_t = sinks::asynchronous_sink<sinks::text_file_backend>;
using file_sink_ptr_t = boost::shared_ptr<file_sink_t>;
using debug_sink_t = sinks::asynchronous_sink<sinks::text_ostream_backend>;using debug_sink_ptr_t = boost::shared_ptr<debug_sink_t>;
using debug_sink_ptr_t = boost::shared_ptr<debug_sink_t>;
//-------------------------------------------------------------------------------------------- 
// configure attributes
void init_attrs(const config::Configuration &conf)
{ 
  const auto &attrs = conf.attributes;

  for (const auto &attr: attrs)
  {
    using config::Configuration;
    
    if (attr.second)
    {
      if (attr.first == Configuration::AttributesValues::process_id)
      {       
        log::core::get()->add_global_attribute(Configuration::AttributesValues::process_id.data(), log::attributes::current_process_id());
      }
      else if (attr.first == Configuration::AttributesValues::thread_id)
      {
        log::core::get()->add_global_attribute(Configuration::AttributesValues::thread_id.data(), log::attributes::current_thread_id());
      }
      else if (attr.first == Configuration::AttributesValues::timestamp)
      {
        if (conf.time_type == Configuration::Time::utc)
        {         
          log::core::get()->add_global_attribute(Configuration::AttributesValues::timestamp.data(), log::attributes::utc_clock());
        }
        else if (conf.time_type == Configuration::Time::local)
        {
          log::core::get()->add_global_attribute(Configuration::AttributesValues::timestamp.data(), log::attributes::local_clock());
        }
        else
        {
          throw std::runtime_error("[logger::init_attributes]: unknown time type of log record.");
        }
      }
      else
      {
        throw std::runtime_error("[logger::init_attributes]: unknown name attribute '" + attr.first + "'");
      }
    }
  }
}
//-------------------------------------------------------------------------------------------- 
// format message
void format(const config::Configuration &conf, const log::record_view &record, log::formatting_ostream &os)
{ 
  auto sev = record.attribute_values()["Severity"].extract<Severity>().get();
  auto text = text_sev[static_cast<int>(sev)];
  
  std::ostringstream tmp;

  auto format_attr = [&tmp](const auto &val)
  {
    tmp << "[" << val << "]";
  };
   
  const auto &attrs = conf.attributes;
  
  if (attrs.at(config::Configuration::AttributesValues::timestamp))
  {
    const auto timestamp = log::extract<boost::posix_time::ptime>("TimeStamp", record).get();
    format_attr(boost::posix_time::to_iso_extended_string(timestamp));
  }

  if (attrs.at(config::Configuration::AttributesValues::thread_id))
  {
    const auto thread_id = log::extract<log::attributes::current_thread_id::value_type>(
        config::Configuration::AttributesValues::thread_id.data(), record).get();
    format_attr(thread_id);
  }

  if (attrs.at(config::Configuration::AttributesValues::process_id))
  {
    const auto process_id = log::extract<log::attributes::current_process_id::value_type>(
        config::Configuration::AttributesValues::process_id.data(), record).get();
    format_attr(process_id);
  } 
  
  format_attr(text);

  const auto msg = record.attribute_values()["Message"].extract<std::string>().get();
  tmp << ": " << msg;

  os << tmp.str();
  
}
//-------------------------------------------------------------------------------------------- 
file_sink_ptr_t createFileSink(const config::Configuration &conf)
{
  namespace fs = boost::filesystem;
  
  fs::path filename;
  if (fs::exists(conf.workdir))
  {
    filename /= conf.workdir;
    filename /= conf.filename;
  }
  else
  {
    filename /= fs::current_path();
    filename /= conf.filename;
  }
  using file_text_backend_t = sinks::text_file_backend;
  using file_text_backend_ptr_t = boost::shared_ptr<file_text_backend_t>;

  file_text_backend_ptr_t backend;

  if (conf.rotation.type == config::Configuration::Rotation::Type::time)
  {
    backend = boost::make_shared<file_text_backend_t>(
        keywords::file_name = filename.string(),
        keywords::time_based_rotation = sinks::file::rotation_at_time_interval(
          boost::posix_time::seconds(conf.rotation.period)));
  }
  else if (conf.rotation.type == config::Configuration::Rotation::Type::size)
  {
    backend = boost::make_shared<file_text_backend_t>(
        keywords::file_name = filename.string(),
        keywords::rotation_size = conf.rotation.size
        );
  }
  else
  {
    throw std::runtime_error("[logger::createFileSink]: unknown type rotation.");
  }

  auto tmp = boost::make_shared<file_sink_t>(backend);
  auto fn = std::bind(format, conf, std::placeholders::_1, std::placeholders::_2);
  tmp->set_formatter(fn);

  return tmp;
}
//-------------------------------------------------------------------------------------------- 
debug_sink_ptr_t createDebugSink(const config::Configuration &conf)
{
  auto debug_backend = boost::make_shared<log::sinks::text_ostream_backend>();
  debug_backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
  debug_backend->auto_flush();
  auto sink = boost::make_shared<debug_sink_t>(debug_backend);
  auto fn = std::bind(format, conf, std::placeholders::_1, std::placeholders::_2);
  sink->set_formatter(fn);
  
  return sink;
}
//-------------------------------------------------------------------------------------------- 
// init logger
void initLog(const config::Configuration &conf)
{
  // add attributes
  init_attrs(conf);

  auto core = log::core::get();

  // create file sink
  auto file_sink = createFileSink(conf);
 
  if (conf.stdoutput)
  {
    // creating debug log
    auto debug_sink = createDebugSink(conf);

    core->add_sink(debug_sink);
  }

  core->add_sink(file_sink);
}
//-------------------------------------------------------------------------------------------- 
/** @brief implementation of the class Logger */
void Logger::init()
{
  // default settings
  initLog(config::Configuration()); 
}
//-------------------------------------------------------------------------------------------- 
void Logger::initFromFile(const boost::filesystem::path &filename)
{
  const auto conf = config::readFile(filename);
  initLog(conf);
}
//-------------------------------------------------------------------------------------------- 
Logger::~Logger()
{
  log::core::get()->remove_all_sinks();
}
//--------------------------------------------------------------------------------------------
}
}
}

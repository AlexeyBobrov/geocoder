/** @file geocoderbase.cpp
 *  @brief the implementation of the class GeocoderBase
 *  @author Bobrov A.E.
 *  @date 16.09.2016
 */

// std
#include <iomanip>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/format.hpp>

// this
#include "geo/geocoderbase.h"
#include "utils/libcurl/libcurl.h"
#include "utils/logger/logger.h"

namespace geocoder
{
namespace geo
{
//--------------------------------------------------------------------------------------------
GeocoderBase::GeocoderBase(GeocoderBase &&) = default;
GeocoderBase &GeocoderBase::operator=(GeocoderBase &&) = default;
GeocoderBase::~GeocoderBase() = default;
//--------------------------------------------------------------------------------------------
class GeocoderBase::Impl final
{
  static const std::size_t timeout_;
  static const std::size_t conntimeout_;
  static const bool verbose_;
public:
  explicit Impl(const boost::property_tree::ptree &conf)
  { 
    auto &logger = geo_logger::get();
    BOOST_LOG_SEV(logger, utils::logger::Severity::info) << "[GeocoderBase::Impl::Impl]: Start initialization geocoder...";

    name_ = conf.get<std::string>("name");
    if (const auto conn = conf.get_child_optional("connection"))
    {
      if (const auto url_opt = conn->get_optional<std::string>("url"))
      {
        const auto timeout = conn->get<std::size_t>("timeout", timeout_);
        const auto conntimeout = conn->get<std::size_t>("conntimeout", conntimeout_);
        const auto verbose = conn->get<bool>("verbose", verbose_);

        BOOST_LOG_SEV(logger, utils::logger::Severity::info) << "[GeocoderBase::Impl::Impl]: Geocoder param: ";
        BOOST_LOG_SEV(logger, utils::logger::Severity::info) << "[GeocoderBase::Impl::Impl]: name '" << name_ << "'";
        BOOST_LOG_SEV(logger, utils::logger::Severity::info) << "[GeocoderBase::Impl::Impl]: timeout '" << timeout << "'";
        BOOST_LOG_SEV(logger, utils::logger::Severity::info) << "[GeocoderBase::Impl::Impl]: conntimeout '" << conntimeout << "'";
        BOOST_LOG_SEV(logger, utils::logger::Severity::info) << "[GeocoderBAse::Impl::Impl]: verbose '" << std::boolalpha << verbose << "'";

        conn_param_ = std::make_tuple(*url_opt, timeout, conntimeout, verbose);
      }
      else
      {
        throw std::runtime_error("[GecoderBase::Impl::Impl]: Failed initialization, is not set 'url' option");
      }
    }
    else
    {
      throw std::runtime_error("[GeocoderBase::Impl::Impl]: Failed initialization, is not exists config section 'connection'");
    }
    
    BOOST_LOG_SEV(logger, utils::logger::Severity::info) << "[GeocoderBase:Impl::Impl]: Complete initization.";
  }
  Impl(Impl &&) = default;
  Impl &operator=(Impl &&) = default;
  Impl(const Impl &) = delete;
  Impl &operator=(const Impl &) = delete;

  std::tuple<std::string, long> get(const std::string &addr)
  {
    std::string url;
    std::size_t timeout {};
    std::size_t conntimeout {};
    bool verbose {};

    std::tie(url, timeout, conntimeout, verbose) = conn_param_;
  
    curl_.setTimeOut(timeout);
    curl_.setConnTimeOut(conntimeout);
    curl_.verbose(verbose);
    
    const auto request = url + addr;
    
    auto &logger = geo_logger::get();
    BOOST_LOG_SEV(logger, utils::logger::Severity::trace) << "[GeocoderBase::Impl::Impl]: resuest '" << request << "'";

    return curl_.get(request);
  }
  
  const std::string &getName() const { return name_; }
private:
  utils::curl::LibCurl curl_;
  std::string name_;
  std::tuple<std::string, std::size_t, std::size_t, bool> conn_param_;
};
//--------------------------------------------------------------------------------------------
const std::size_t GeocoderBase::Impl::timeout_ = 100;
const std::size_t GeocoderBase::Impl::conntimeout_ = 100;
const bool GeocoderBase::Impl::verbose_ = false;
//-------------------------------------------------------------------------------------------- 
GeocoderBase::GeocoderBase(const boost::property_tree::ptree &conf)
  : impl_(new Impl(conf))
{
}
//-------------------------------------------------------------------------------------------- 
GeocoderBase::Result GeocoderBase::geocode(const std::string &address)
{
  // get data from geocoder
  std::string buffer;
  long code {};
  std::tie(buffer, code) = impl_->get(address);

  // > 400 - error clients
  if (code > 400)
  {
    boost::format err("[GeocoderBase::geocoder]: failed read data from geocoder '%1%', address = '%2%', return code '%3%'");
    err % impl_->getName();
    err % address;
    err % code;
    throw std::runtime_error(err.str());
  }

  return parse(buffer);
}
//-------------------------------------------------------------------------------------------- 
}
}

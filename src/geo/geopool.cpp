/** @file geopool.cpp
 *  @brief the implementation of the class GeoPool
 *  @author Bobrov A.E.
 *  @date 16.09.2016
 */

// std
#include <algorithm>

// boost
#include <boost/property_tree/ptree.hpp>

// this
#include "geo/geopool.h"
#include "geo/geocoderbase.h"
#include "geo/geoyandex.h"
#include "utils/logger/logger.h"

namespace geocoder
{
namespace geo
{
//-------------------------------------------------------------------------------------------- 
GeoPool::GeoPool(GeoPool &&) = default;
GeoPool &GeoPool::operator=(GeoPool &&) = default;
GeoPool::~GeoPool() = default;
//-------------------------------------------------------------------------------------------- 
class GeoPool::Impl final
{
public:
  explicit Impl(const boost::property_tree::ptree &conf)
  {
    using utils::logger::Severity;
    auto &logger = geo_logger::get();
    BOOST_LOG_SEV(logger, Severity::info) << "[GeoPool::Impl::Impl]: Start initialization GeoPool...";

    if (const auto g = conf.get_child_optional("document.geocoders"))
    {
      auto r = g->equal_range("geocoder");
      for (; r.first != r.second; ++r.first)
      {
        const auto &geo = r.first->second;
        const auto name = geo.get<std::string>("name");
        if (name == "yandex")
        {
          geocoders_.push_back(createYandexGeocoder(geo));
        }
      }
    }
    else
    {
      BOOST_LOG_SEV(logger, Severity::warning) << "[GeoPool::Impl::Impl]: is not found 'document.geocoders'";
    }


    BOOST_LOG_SEV(logger, Severity::info) << "[GeoPool::Impl::Impl]: Complete initialization.";
  }

  Answer get(const std::string &addr)
  {
    Answer result;

    for (const auto &g: geocoders_)
    {
      try
      {
        auto ret = g->geocode(addr);
        if (std::get<0>(ret))
        {
          std::swap(result, std::get<1>(ret));
          break;
        }
      }
      catch (const std::exception &err)
      {
        auto &logger = geo_logger::get();
        BOOST_LOG_SEV(logger, utils::logger::Severity::warning) << "[GeoPool::Impl::get]: failed request data, '"
          << err.what() << "'";
      }
    }

    return result;
  }
  
  Impl(Impl &&) = default;
  Impl &operator=(Impl &&) = default;
  Impl(const Impl &) = delete;
  Impl &operator=(const Impl &) = delete;
private:
  std::vector<GeocoderPtr> geocoders_;
};
//-------------------------------------------------------------------------------------------- 
GeoPool::GeoPool(const boost::property_tree::ptree &conf)
  : impl_(new Impl(conf))
{
}
//-------------------------------------------------------------------------------------------- 
Answer GeoPool::geocode(const std::string &addr)
{
  return impl_->get(addr);
}
//-------------------------------------------------------------------------------------------- 
}
}

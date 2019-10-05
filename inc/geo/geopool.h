/** @file geopool.h
 *  @brief The define of the class GeoPool
 *  @author Bobrov A.E.
 *  @date 16.09.2016
 */
#ifndef GEOCODER_GEO_GEOPOOL_H_
#define GEOCODER_GEO_GEOPOOL_H_

// std
#include <memory>
#include <string>

// boost
#include <boost/property_tree/ptree.hpp>

// this
#include "geo/answer.h"
#include "geo/location.h"

namespace geocoder
{
namespace geo
{
class GeoPool final
{
 public:
  explicit GeoPool(const boost::property_tree::ptree &conf);
  GeoPool(GeoPool &&);
  GeoPool &operator=(GeoPool &&);
  GeoPool(const GeoPool &) = delete;
  GeoPool &operator=(const GeoPool &) = delete;
  ~GeoPool();
  Answer geocode(const std::string &address);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
}  // namespace geo
}  // namespace geocoder

#endif

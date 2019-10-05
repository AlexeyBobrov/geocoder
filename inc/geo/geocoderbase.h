/** @file geocoderbase.h
 *  @brief the define of the class GeocoderBase
 *  @author Bobrov A.E.
 *  @date 16.09.2016
 */
#ifndef GEOCODER_GEO_GEOCODERBASE_H_
#define GEOCODER_GEO_GEOCODERBASE_H_

// std
#include <memory>
#include <string>
#include <tuple>

// boost
#include <boost/property_tree/ptree_fwd.hpp>

// this
#include "geo/answer.h"

namespace geocoder
{
namespace geo
{
class GeocoderBase
{
 public:
  using Result = std::tuple<bool, Answer>;

 public:
  explicit GeocoderBase(const boost::property_tree::ptree &conf);
  GeocoderBase(GeocoderBase &&);
  GeocoderBase &operator=(GeocoderBase &&);
  GeocoderBase(const GeocoderBase &) = delete;
  GeocoderBase &operator=(const GeocoderBase &) = delete;
  ~GeocoderBase();
  Result geocode(const std::string &address);

 protected:
  virtual Result parse(const std::string &buffer) = 0;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

using GeocoderPtr = std::unique_ptr<GeocoderBase>;
}  // namespace geo
}  // namespace geocoder

#endif

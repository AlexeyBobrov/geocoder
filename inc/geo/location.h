/** @file location.h
 *  @brief the define of the struct Location
 *  @author Bobrov A.E.
 *  @date 15.09.2016
 **/
#ifndef GEOCODER_GEO_LOCATION_H_
#define GEOCODER_GEO_LOCATION_H_

// std
#include <stdexcept>
#include <string>
#include <vector>

namespace geocoder
{
namespace geo
{
struct Coordinates
{
  /** @brief to gcc >= 5 to delete */
  Coordinates(double lat = 1000.0, double lon = 1000.0)
   : latitude(lat)
   , longitude(lon)
  {
  }

  double latitude{1000.0};
  double longitude{1000.0};

  bool isValid() const { return ((longitude >= -180.0 && longitude <= 180.0) && (latitude >= -90.0 && latitude <= 90)); }
};

enum class Precision
{
  exact = 0,
  number,
  nearly,
  range,
  street,
  other
};

inline std::string PrecisionToString(Precision p)
{
  switch (p)
  {
    case Precision::exact:
      return "exact";
    case Precision::number:
      return "number";
    case Precision::nearly:
      return "nearly";
    case Precision::range:
      return "range";
    case Precision::street:
      return "street";
    case Precision::other:
      return "other";
    default:
      throw std::runtime_error("[PrecisionToString]: unknown precision type");
  }
}

inline Precision StringToPrecision(const std::string &prec)
{
  if (prec == "exact")
  {
    return Precision::exact;
  }
  else if (prec == "number")
  {
    return Precision::number;
  }
  else if (prec == "nearly")
  {
    return Precision::nearly;
  }
  else if (prec == "range")
  {
    return Precision::range;
  }
  else if (prec == "street")
  {
    return Precision::street;
  }
  else if (prec == "other")
  {
    return Precision::other;
  }
  else
  {
    throw std::runtime_error("[StringToPrecision]: unknown text precision");
  }
}

struct Location
{
  std::string line;
  std::string country;
  std::string region;
  std::string district;
  std::string place;
  std::string suburb;
  std::string street;
  std::string house;
  Coordinates coord;
  Precision precision = Precision::other;
};

using Locations = std::vector<Location>;
}  // namespace geo
}  // namespace geocoder

#endif

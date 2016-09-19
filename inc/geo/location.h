/** @file location.h
 *  @brief the define of the struct Location
 *  @author Bobrov A.E.
 *  @date 15.09.2016
 **/
#ifndef GEOCODER_GEO_LOCATION_H_
#define GEOCODER_GEO_LOCATION_H_

// std
#include <string>
#include <vector>

namespace geocoder
{
namespace geo
{
struct Coordinates
{
  double latitude {1000.0};
  double longitude {1000.0};

  bool isValid() const
  {
    return ((longitude >= -180.0 && longitude <= 180.0) &&
        (latitude >= -90.0 && latitude <= 90));
  }
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
}
}

#endif

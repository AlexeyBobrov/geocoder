/** @file answer.h
 *  @brief the define of the struct Answer
 *  @author Bobrov A.E.
 *  @date 15.09.2016
 */
#ifndef GEOCODER_GEO_ANSWER_H_
#define GEOCODER_GEO_ANSWER_H_

// this
#include "geo/location.h"

namespace geocoder
{
namespace geo
{
struct Answer
{
  enum class GeocoderType
  {
    unknown = -1,
    yandex = 0
  };

  static inline std::string GeoTypeToText(GeocoderType type)
  {
    switch (type)
    {
      case GeocoderType::yandex:
        return "yandex";
      case GeocoderType::unknown:
        return "unknown";
      default:
        throw std::runtime_error("[Answer::GeoTypeToText]: unknown type geocoder");
    }
  }
  
  Locations locations;
  GeocoderType type = GeocoderType::unknown;
};
}
}

#endif

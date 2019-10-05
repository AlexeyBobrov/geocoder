/** @file geoyandex.h
 *  @brief the define of the function factory geocoder yandex
 *  @author Bobrov A.E.
 *  @date 16.09.2016
 */
#ifndef GEOCODER_GEO_GEOYANDEX_H_
#define GEOCODER_GEO_GEOYANDEX_H_

// this
#include "geo/geocoderbase.h"

namespace geocoder
{
namespace geo
{
GeocoderPtr createYandexGeocoder(const boost::property_tree::ptree &conf);
}
}  // namespace geocoder

#endif

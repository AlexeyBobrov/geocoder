/** @file geoyandex.cpp
 *  @brief the implementation of the class GeoYandex
 *  @author Bobrov A.E.
 *  @date 16.09.2016
 */

// std
#include <sstream>
#include <map>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>

// this
#include "geo/geoyandex.h"
#include "utils/logger/logger.h"

namespace geocoder
{
namespace geo
{

using GeoData = std::map<std::string, std::string>;

void processTree(const boost::property_tree::ptree &pt, GeoData &data)
{
  for (const auto &i: pt.get_child(""))
  {
    const auto &name = i.first;
    auto desc = pt.get<std::string>(name);

    boost::algorithm::trim(desc);
    if (!desc.empty())
    {
      data.emplace(name, desc);
    }

    processTree(i.second, data);
  }
}

Precision textToPrecision(const std::string &precision)
{
  if (precision == "exact")
  {
    return Precision::exact;
  }
  else if (precision == "number")
  {
    return Precision::number;
  }
  else if (precision == "nearly")
  {
    return Precision::nearly;
  }
  else if (precision == "range")
  {
    return Precision::range;
  }
  else if (precision == "street")
  {
    return Precision::street;
  }
  else if (precision == "other")
  {
    return Precision::other;
  }
  else 
  {
    throw std::runtime_error("[textToPrecision]: is not found precision '" + precision + "'");
  }
}

class GeoYandex final : public GeocoderBase
{
public:
  explicit GeoYandex(const boost::property_tree::ptree &conf)
    : GeocoderBase(conf)
  {
  }
protected:
  virtual Result parse(const std::string &buffer)
  {
    namespace pt = boost::property_tree;

    std::istringstream in(buffer);

    Answer answer;
    answer.type = Answer::GeocoderType::yandex;
    
    pt::ptree document;
    pt::read_xml(in, document);

    if (const auto ymaps = document.get_child_optional("ymaps"))
    {
      if (const auto geo_obj_coll = ymaps->get_child_optional("GeoObjectCollection"))
      {
        auto r = geo_obj_coll->equal_range("featureMember");

        for (; r.first != r.second; ++r.first)
        {
          const auto geo_obj = r.first->second;
          GeoData data;
          processTree(geo_obj, data);

          auto text_coord = data["pos"];
          std::vector<std::string> spl_coord;
          boost::split(spl_coord, text_coord, boost::is_any_of(" "));
          
          Coordinates coord;
          coord.longitude = std::stof(spl_coord.at(0));
          coord.latitude = std::stof(spl_coord.at(1));         

          Location loc;
          loc.line = data["AddressLine"];
          loc.country = data["CountryName"];
          loc.region = data["AdministrativeAreaName"];
          loc.district = data["SubAdministrativeAreaName"];
          loc.place = data["LocalityName"];
          loc.suburb = data["DependentLocalityName"];
          loc.street = data["ThoroughfareName"];
          loc.house = data["PremiseNumber"];
          loc.coord = coord;
          loc.precision = textToPrecision(data["precision"]);

          answer.locations.push_back(std::move(loc));

        }
      }
    }
   
    Result result;
    bool ret = (!answer.locations.empty()) ? true : false;

    return std::make_tuple(ret, answer);
  } 
};

GeocoderPtr createYandexGeocoder(const boost::property_tree::ptree &conf)
{
  GeocoderPtr result(new GeoYandex(conf));
  return result;
}
}
}


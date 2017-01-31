/** @file test_geocoder.cpp
 *  @brief the implementation test for geocoder
 *  @author Bobrov A.E.
 *  @date 20.09.2016
 */

// std
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <stdexcept>
#include <algorithm>

// boost
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>

// this
#include "geo/location.h"
#include "geo/geopool.h"
#include "test_utils.h"

using LocationLine = std::pair<std::string, geocoder::geo::Location>;
using DataSet = std::vector<LocationLine>;

BOOST_AUTO_TEST_SUITE(test_geocoder)


BOOST_AUTO_TEST_CASE(test_geopool)
{
  
  namespace fs = boost::filesystem;
  // read addrs from file
  fs::path addr_file {"../test/addrs.txt"};

  if (!fs::exists(addr_file))
  {
    throw std::runtime_error("[gest_geopool]: is not exists file '" + addr_file.string() + "'");
  }

  auto addrs = geocoder::test::readFromFile(addr_file);

  namespace pt = boost::property_tree;
  pt::ptree document;
  fs::path conf {"../config/geocoder.xml"};
  pt::read_xml(conf.string(), document);

  geocoder::geo::GeoPool pool(document);

  
  for (const auto &i: addrs)
  {
    auto ret = pool.geocode(i.first);
    
    // remove elements not exact precision
    auto it = std::remove_if(std::begin(ret.locations), std::end(ret.locations),
        [](const auto &i)
        {
          return (i.precision != geocoder::geo::Precision::exact); 
        });

    ret.locations.erase(it, std::end(ret.locations));
    const auto &locs = ret.locations;
    BOOST_REQUIRE(locs.size() == 1);

    const auto &loc = locs.front();   
    BOOST_CHECK_EQUAL(loc.line, i.second.line);
    BOOST_CHECK_EQUAL(loc.country, i.second.country);
    BOOST_CHECK_EQUAL(loc.region, i.second.region);
    BOOST_CHECK_EQUAL(loc.district, i.second.district);
    BOOST_CHECK_EQUAL(loc.place, i.second.place);
    BOOST_CHECK_EQUAL(loc.suburb, i.second.suburb);
    BOOST_CHECK_EQUAL(loc.street, i.second.street);
    BOOST_CHECK_EQUAL(loc.house, i.second.house);
    BOOST_REQUIRE(loc.precision ==i.second.precision);   


    BOOST_TEST_MESSAGE("successfully check request '" << i.first << "'");
  }
  
}

BOOST_AUTO_TEST_SUITE_END()

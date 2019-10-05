/** @file test_utils.cpp
 *  @brief the implementation of the helpers function for test
 *  @author Bobrov A.E.
 *  @date 21.09.2016
 */
// declare
#include "test_utils.h"

// std
#include <fstream>
#include <vector>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

namespace geocoder
{
namespace test
{
//--------------------------------------------------------------------------------------------
DataSet readFromFile(const boost::filesystem::path &filename)
{
  namespace fs = boost::filesystem;

  if (!fs::exists(filename))
  {
    throw std::runtime_error("[readFromFile]: is not found file '" + filename.string() + "'");
  }

  std::ifstream fin(filename.string());

  if (!fin.is_open())
  {
    throw std::runtime_error("[readFromFile]: is not open file '" + filename.string() + "'");
  }

  DataSet result;

  std::string line;
  while (std::getline(fin, line))
  {
    std::vector<std::string> row;

    boost::split(row, line, boost::is_any_of(";"));

    BOOST_CHECK(row.size() == 12);

    const auto &line = row[0];

    geocoder::geo::Location loc;
    loc.line = row[1];
    loc.country = row[2];
    loc.region = row[3];
    loc.district = row[4];
    loc.place = row[5];
    loc.suburb = row[6];
    loc.street = row[7];
    loc.house = row[8];
    loc.coord.latitude = std::stod(row[9]);
    loc.coord.longitude = std::stod(row[10]);
    loc.precision = geocoder::geo::StringToPrecision(row[11]);

    result.emplace(line, loc);
  }

  return result;
}
//--------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace geocoder

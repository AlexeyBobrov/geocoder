/** @file test_utils.h
 *  @brief test utils
 *  @author Bobrov A.E.
 *  @date 21.09.2016
 */
#ifndef GEOCODER_TEST_TEST_UTILS_H_
#define GEOCODER_TEST_TEST_UTILS_H_

// std
#include <map>

// boost
#include <boost/filesystem.hpp>

// this
#include "geo/location.h"

namespace geocoder
{
namespace test
{
using DataSet = std::map<std::string, geo::Location>;

/** @brief read data set from file */
DataSet readFromFile(const boost::filesystem::path &filename);

}  // namespace test
}  // namespace geocoder

#endif

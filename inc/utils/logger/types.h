/** @file types.h
 * @brief the define log types
 * @author Bobrov A.E.
 * @date 09.07.2016
 */
#ifndef GEOCODER_COMMON_LOGGER_TYPES_H_
#define GEOCODER_COMMON_LOGGER_TYPES_H_

// boost
#include <boost/property_tree/ptree_fwd.hpp>

namespace geocoder
{
namespace utils
{
namespace logger
{
using Config = boost::property_tree::ptree;
}
}  // namespace utils
}  // namespace geocoder

#endif

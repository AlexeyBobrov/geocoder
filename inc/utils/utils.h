/** @file utils.h
 *  @brief the helpers function
 *  @author Bobrov A.E.
 *  @date 17.09.2016
 */
#ifndef GEOCODER_UTILS_UTILS_H_
#define GEOCODER_UTILS_UTILS_H_

// std
#include <string>

// boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

namespace geocoder
{
namespace utils
{
/** @brief generate filename
 * @param prefix - prefix name
 * @param ext - extension filename
 * @param t - time
 * @param mask - mask time
 * @return filename
 */
std::string generateFileName(const std::string &prefix, const std::string &ext,
                             const boost::posix_time::ptime &t = boost::posix_time::second_clock::universal_time(),
                             const std::string &mask = "%Y%m%d_%H%M%S");
/** @brief get real filename
 *  @details not exists filename - current directory
 *  @param filename - filename
 *  @return real fullname filename
 */
boost::filesystem::path getRealFileName(const boost::filesystem::path &filename);
}  // namespace utils
}  // namespace geocoder

#endif

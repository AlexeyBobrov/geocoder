/** @file libcurl.h
  * @brief the define of the class LibCurl
  * @author Bobrov A.E.
  * @date 16.07.2016
  */
#ifndef GEOCODER_UTILS_LIBCURL_LIBCURL_H_
#define GEOCODER_UTILS_LIBCURL_LIBCURL_H_

// std
#include <memory>
#include <string>
#include <set>
#include <tuple>

// boost
#include <boost/property_tree/ptree_fwd.hpp>

namespace geocoder
{
namespace utils
{
namespace curl
{

using Headers = std::set<std::string>;

/** @class LibCurl
  * @brief The define wrapper libcurl library (curl), thread safe (internal lock)
  * @url https://curl.haxx.se//
  */
class LibCurl final
{
public:
  /** @brief default ctor */
  LibCurl();
  /** @brief disable copy semantics */
  LibCurl(const LibCurl &) = delete;
  LibCurl &operator=(const LibCurl &) = delete;
  /** @brief move semantics */
  LibCurl(LibCurl &&);
  LibCurl &operator=(LibCurl &&);
  /** @brief dtor */
  ~LibCurl();
  /** @brief http get request 
    * @param url - url 
    * @return tuple<std::string - answer, long - response code> 
    */
  std::tuple<std::string, long> get(const std::string &url);
  /** @brief http post request
    * @param url - url
    * @return tuple<std::string - answer, long - response code> 
    */
  std::tuple<std::string, long> post(const std::string &url);
  /** @brief enable verbose */
  void verbose(bool enable = true);
  /** @brief set timeout (seconds) */
  void setTimeOut(std::uint32_t t);
  /** @brief set connection timeout (seconds) */
  void setConnTimeOut(std::uint32_t t);
  /** @brief set headers
    * @param encoding - encoding (UTF, CP1251)
    * @param headers - other headers
    */
  void setHeaders(const std::string &encoding, const Headers &h);
  /** @brief set login and password
    * @param login - login
    * @param password - password
    */
  void setUserPassw(const std::string &login, const std::string &passw);
  /** @brief escaping url
    * @param url - url
    * @return escape result
    */
  std::string escapeUrl(const std::string &url);
private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
}
}
}

#endif

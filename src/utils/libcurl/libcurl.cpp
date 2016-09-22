/** @file libcurl.cpp
  * @brief The implementation libcurl wrapper
  * @author Bobrov A.E.
  * @date 16.07.2016
  */
// this
#include "utils/libcurl/libcurl.h"

// std
#include <memory>
#include <functional>
#include <cassert>
#include <array>
#include <sstream>
#include <mutex>
#include <cstdint>

// curl
#include <curl/curl.h>


namespace geocoder
{
namespace utils
{
namespace curl
{
// RAII for curl
using TCurlCleaner = std::function<void(CURL *)>;
using CurlPtr = std::unique_ptr<CURL, TCurlCleaner>;
auto CurlCleaner(CURL *p)
{
  assert(p && "Is not nullptr curl pointer");
  curl_easy_cleanup(p);
}
using TCurlListCleaner = std::function<void(curl_slist *)>;
using CurlListPtr = std::unique_ptr<curl_slist, TCurlListCleaner>;
auto CurlListCleaner(curl_slist *t)
{
  if (t)
  {
    curl_slist_free_all(t);
  }
}

/** @class CurlGlobalInitializator
  * @brief the initalizator class
  */
class CurlGlobalInitializator final
{
public:
  CurlGlobalInitializator()
  {
    auto ret = curl_global_init(CURL_GLOBAL_ALL);

    if (CURLE_OK == ret)
    {
      init_ = true;
    }
  }
  ~CurlGlobalInitializator()
  {
    if (init_)
    {
      curl_global_cleanup();
    }
  }

  bool isInitialization() const { return init_; }
  operator bool() const { return init_; }
private:
  bool init_{ false };
};

LibCurl::LibCurl(LibCurl &&) = default;
LibCurl &LibCurl::operator=(LibCurl &&) = default;
LibCurl::~LibCurl() = default;

class LibCurl::Impl final
{
public:
  Impl()
    : curl_{nullptr, CurlCleaner}
    , curl_list_{nullptr, CurlListCleaner}
  {
    static CurlGlobalInitializator initializator;

    if (!initializator)
    {
      throw std::runtime_error("[LibCurl::init]: failed global initialization.");
    }

    CURL *p = curl_easy_init();
    if (!p)
    {
      throw std::runtime_error("[LibCurl::init]: failed 'curl_easy_init()'.");
    }

    curl_.reset(p);
  }

  //----------------------------------------------------------------------------------------
  void verbose(bool enable)
  {
    std::unique_lock<std::mutex> locker(lock_);

    curlSetOpt(CURLOPT_VERBOSE, static_cast<long>(enable), "LibCurl::verbose", "CURLOPT_VERBOSE");
  }
  //----------------------------------------------------------------------------------------
  void setTimeOut(std::uint32_t t)
  {
    std::unique_lock<std::mutex> locker(lock_);

    curlSetOpt(CURLOPT_TIMEOUT, static_cast<long>(t), "LibCurl::setTimeOut", "CURLOPT_TIMEOUT");
  }
  //----------------------------------------------------------------------------------------
  void setConnTimeOut(std::uint32_t t)
  {
    std::unique_lock<std::mutex> locker(lock_);

    curlSetOpt(CURLOPT_CONNECTTIMEOUT, static_cast<long>(t), "LibCurl::setConnTimeOut", "CURLOPT_CONNECTTIMEOUT");
  }
  //----------------------------------------------------------------------------------------
  void setUserPassw(const std::string &login, const std::string &passw)
  {
    std::unique_lock<std::mutex> locker(lock_);

    curlSetOpt(CURLOPT_USERNAME, login.data(), "LibCurl::setUserPassw", "CURLOPT_USERNAME");
    curlSetOpt(CURLOPT_PASSWORD, passw.data(), "LibCurl::setUasePassw", "CURLOPT_PASSWORD");
  }
  //----------------------------------------------------------------------------------------
  void setHeaders(const std::string &encoding, const Headers &h)
  {
    std::unique_lock<std::mutex> locker(lock_);

    curlSetOpt(CURLOPT_ENCODING, encoding.c_str(), "LibCurl::setHeaders", "CURLOPT_ENCODING");

    curl_slist *headers = nullptr;
    for (const auto &i : h)
    {
      headers = curl_slist_append(headers, i.data());
    }

    curl_list_.reset(headers);
    
    curlSetOpt(CURLOPT_HTTPHEADER, curl_list_.get(), "LibCurl::setHeaders", "CURLOPT_HTTPHEADER");
  }
  //----------------------------------------------------------------------------------------
  std::tuple<std::string, long> get(const std::string &url)
  {
    std::unique_lock<std::mutex> locker(lock_);

    curlSetOpt(CURLOPT_URL, url.data(), "LibCurl::get", "CURLOPT_URL");
    std::array<char, CURL_ERROR_SIZE> errBuffer;
    curlSetOpt(CURLOPT_ERRORBUFFER, errBuffer.data(), "LibCurl::get", "CURLOPT_ERRORBUFFER");
    
    curlSetOpt(CURLOPT_WRITEFUNCTION, writer, "LibCurl::get", "CURLOPT_WRITEFUNCTION");

    std::string buffer;
    curlSetOpt(CURLOPT_WRITEDATA, &buffer, "LibCurl::get", "CURLOPT_WRITEDATA");

    if (curl_list_)
    {
      curlSetOpt(CURLOPT_HEADER, 1, "LibCurl::get", "CURLOPT_HEADER");
    }
    
    auto responseCode = static_cast<long>(0);
    auto result = curl_easy_perform(curl_.get());
    auto retGetInfo = curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &responseCode);
    if (CURLE_OK != retGetInfo)
    {
      throwCurlError(retGetInfo, "LibCurl::get", "CURLINFO_RESPONSE_CODE");
    }

    curl_easy_reset(curl_.get());
    curl_list_.reset();
    
    if (CURLE_OK != result)
    {
      throwCurlError(result, "LibCurl::get", std::string());
    }

    return std::make_tuple(std::move(buffer), responseCode);
  }
  //----------------------------------------------------------------------------------------
  std::tuple<std::string, long> post(const std::string &url)
  {
    std::unique_lock<std::mutex> locker(lock_);

    curlSetOpt(CURLOPT_URL, url.data(), "LibCurl::post", "CURLOPT_URL");
    std::array<char, CURL_ERROR_SIZE> errBuffer;
    curlSetOpt(CURLOPT_ERRORBUFFER, errBuffer.data(), "LibCurl::post", "CURLOPT_ERRORBUFFER");
    curlSetOpt(CURLOPT_POST, 1L, "LibCurl::get", "CURLOPT_POST");
    
    curlSetOpt(CURLOPT_READFUNCTION, writer, "LibCurl::post", "CURLOPT_READFUNCTION");

    std::string buffer;
    curlSetOpt(CURLOPT_READDATA, &buffer, "LibCurl::post", "CURLOPT_READDATA");

    if (curl_list_)
    {
      curlSetOpt(CURLOPT_HEADER, 1, "LibCurl::post", "CURLOPT_HEADER");
    }
    
    auto responseCode = static_cast<long>(0);
    auto result = curl_easy_perform(curl_.get());
    auto retGetInfo = curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &responseCode);
    if (CURLE_OK != retGetInfo)
    {
      throwCurlError(retGetInfo, "LibCurl::post", "CURLINFO_RESPONSE_CODE");
    }

    curl_easy_reset(curl_.get());
    curl_list_.reset();
    
    if (CURLE_OK != result)
    {
      throwCurlError(result, "LibCurl::post", std::string());
    }

    return std::make_tuple(std::move(buffer), responseCode);

  }
  //----------------------------------------------------------------------------------------
  std::string escapeUrl(const std::string &url)
  {
    std::unique_lock<std::mutex> locker(lock_);

    char *ptr = curl_easy_escape(curl_.get(), url.data(), url.size());
    std::string result{ ptr };
    curl_free(ptr);
    return result;
  }
  //---------------------------------------------------------------------------------------
  /** @brief disable copy semantics */
  Impl(const Impl &) = delete;
  Impl &operator=(const Impl &) = delete;
  /** @brief move semantics */
  Impl(Impl &&) = default;
  Impl &operator=(Impl &&) = default;
  ~Impl() = default;
  
private:
  /** @brief throw curl error
    * @param code - return code libcurl function
    * @param source - source error
    * @param optname - name option
    */
  void throwCurlError(CURLcode code, const std::string &source, const std::string &optname)
  {
    auto str = curl_easy_strerror(code);
    std::ostringstream err;

    err << "[" << source << "]: ";
    if (!optname.empty())
    {
      err << "option name '" << optname + "', ";
    }
    
    err << str;
    
    throw std::runtime_error(err.str());
  }

  void throwCurlError(CURLcode code, std::string &&source, std::string &&optname)
  {
    throwCurlError(code, source, optname);
  }

  template <typename T>
  void curlSetOpt(std::uint32_t opt, T t, std::string &&source, std::string &&optname)
  {
    auto ret = curl_easy_setopt(curl_.get(), static_cast<CURLoption>(opt), t);
    if (CURLE_OK != ret)
    {
      throwCurlError(ret, source, optname);
    }
  }

  /** @brief callback writer function */
  static std::size_t writer(char *data, std::size_t size, std::size_t nmemb, void *buffer)
  {
    auto result = static_cast<std::size_t>(0);

    std::string *ptr = static_cast<std::string*>(buffer);

    if (ptr)
    {
      ptr->append(data, size * nmemb);
      result = size * nmemb;
    }

    return result;
  }
  
private:
  CurlPtr curl_;
  CurlListPtr curl_list_;
  std::mutex lock_;
};

////////////////////////////////////////////////////////////////////////////////
LibCurl::LibCurl()
  : impl_(new Impl())
{

}
//------------------------------------------------------------------------------
std::tuple<std::string, long> LibCurl::get(const std::string &url)
{
  return impl_->get(url);
}
//------------------------------------------------------------------------------
std::tuple<std::string, long> LibCurl::post(const std::string &url)
{
  return impl_->post(url);
}
//------------------------------------------------------------------------------
void LibCurl::verbose(bool enable)
{
  impl_->verbose(enable);
}
//------------------------------------------------------------------------------
void LibCurl::setTimeOut(std::uint32_t t)
{
  impl_->setTimeOut(t);
}
//------------------------------------------------------------------------------
void LibCurl::setConnTimeOut(std::uint32_t t)
{
  impl_->setConnTimeOut(t);
}
//------------------------------------------------------------------------------
void LibCurl::setHeaders(const std::string &encoding, const Headers &h)
{
  impl_->setHeaders(encoding, h);
}
//------------------------------------------------------------------------------
void LibCurl::setUserPassw(const std::string &login, const std::string &passw)
{
  impl_->setUserPassw(login, passw);
}
//------------------------------------------------------------------------------
std::string LibCurl::escapeUrl(const std::string &url)
{
  return impl_->escapeUrl(url);
}
//------------------------------------------------------------------------------
}
}
}

/** @file utils.cpp
 *  @brief the implementation of the function helper
 *  @author Bobrov A.E.
 *  @date 17.09.2016
 */

// std
#include <iostream>
#include <sstream>

// this
#include "utils/utils.h"

namespace geocoder
{
namespace utils
{
//-------------------------------------------------------------------------------------------- 
std::string generateFileName(const std::string &prefix, const std::string &ext, 
    const boost::posix_time::ptime &t,
    const std::string &mask)
{
  std::locale loc(std::cout.getloc(), new boost::posix_time::time_facet(mask.c_str()));

  std::ostringstream filename;
  filename.imbue(loc);
  
  filename << prefix << "_" << t << "." << ext;

  return filename.str();    
}
//-------------------------------------------------------------------------------------------- 
boost::filesystem::path getRealFileName(const boost::filesystem::path& fileName)
{
  namespace fs = boost::filesystem;
  
  fs::path result;
  if (!fs::exists(fileName.relative_path()))
  {
    result = fs::current_path();
    result /= fileName.filename();
  }
  else
  {
    result = fileName;
  }

  return result;
}
//---------------------------------------------------------------------------------------------
}
}

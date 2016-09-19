/** @file parse_cmd.h
  * @brief the define parse_cmd module
  * @author Bobrov A.E.
  * @date 30.08.2016
  */
#ifndef GEOCODER_UTILS_PARSE_CMD_H_
#define GEOCODER_UTILS_PARSE_CMD_H_

// std
#include <string>

// boost
#include <boost/filesystem.hpp>

namespace geocoder
{
namespace utils
{
/** @brief parse cmd
  * @param argc - number argv
  * @param argv - command line
  * @param [out] config_filename- configuration filename (required)
  * @param [out] address - address (optional)
  * @param [out] addr_filename - file with address
  * @param [out] outfile - output file (optional)
  * @return true - if success parse command line, else = false
  */
bool parseCmd(int argc, char *argv[], boost::filesystem::path &config_filename, 
  std::string &address, boost::filesystem::path &addr_filename, 
  boost::filesystem::path &outfile);
}
}

#endif

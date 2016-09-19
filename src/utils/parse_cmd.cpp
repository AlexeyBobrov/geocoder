/** @file parse_cmd.cpp
  * @brief The implementation of functions parse cmd
  * @author Bobrov A.E.
  * @date 31.08.2016
 */

// std
#include <iostream>

// boost
#include <boost/program_options.hpp>
#include <boost/assert.hpp>

// this
#include "utils/parse_cmd.h"
#include "GeocoderVersion.h"

namespace geocoder
{
namespace utils
{
//--------------------------------------------------------------------------------------------
bool parseCmd(int argc, char *argv[], boost::filesystem::path &config_filename, 
  std::string &address, 
  boost::filesystem::path &addr_filename,
  boost::filesystem::path &outfile)
{
  namespace bp = boost::program_options;

  BOOST_ASSERT_MSG(argc > 0, "count argument is null");
  BOOST_ASSERT_MSG(argv, "command line is nullptr");
  
  bp::options_description option_desc("Allowed options");

  std::string config;
  std::string address_fname;
  std::string addr;
  std::string output;

  option_desc.add_options()
    ("help,h", "Display the program usage and exit")
    ("version,v", "Display the program version and exit")
    ("config,c", bp::value<std::string>(&config)->required(), "Configuration file name (required)")
    ("addr_file,A", bp::value<std::string>(&address_fname), "Address file name (optional)")
    ("address,a", bp::value<std::string>(&addr), "address (optional)")
    ("output,o", bp::value<std::string>(&output), "output file (optional)");

  bp::variables_map options;
  bp::store(bp::command_line_parser(argc, argv).options(option_desc).run(), options);
  if (options.empty() || options.count("help"))
  {
    std::cerr << "Usage: " << argv[0] << " --config=configuration-file" << std::endl << std::endl;
    std::cerr << option_desc << std::endl;
    return false;
  }
  else if (options.count("version"))
  {
    std::cout << geocoder::version::getText() << std::endl;
    return false;
  }
  
  bp::notify(options);

  config_filename = { config };
  addr_filename = { address_fname };
  outfile = { output };
  std::swap(address, addr);

  return true;
}
//--------------------------------------------------------------------------------------------
}
}

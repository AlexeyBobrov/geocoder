/** @file main.cpp
 *  @brief the entry point geocoder program
 *  @author Bobrov A.E.
 *  @date 11.09.2016
 */

// std
#include <iostream>

// boost
#include <boost/filesystem.hpp>

// system
#include <curl/curl.h>

// this
#include "GeocoderVersion.h"
#include "logger/logger.h"
#include "utils/parse_cmd.h"


int main(int argc, char *argv[])
{

  namespace fs = boost::filesystem;
  
  fs::path config_filename;
  fs::path addr_filename;
  std::string addr;

  if (!geocoder::utils::parseCmd(argc, argv, config_filename, addr_filename, addr))
  {
    std::cerr << "[main]: failed parse command line \n";
    return EXIT_FAILURE;
  }

  geocoder::logger::Logger::initFromFile(config_filename);

  auto &logger = geo_logger::get();

  BOOST_LOG_SEV(logger, geocoder::logger::Severity::info) << "Start '" << geocoder::version::getText() << "'";

  try
  {
    
  }
  catch (const std::exception &err)
  {
    BOOST_LOG_SEV(logger, geocoder::logger::Severity::fatal) << "Failed '" << err.what() << "'";
  }

  return 0;
}

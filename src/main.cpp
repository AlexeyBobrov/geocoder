/** @file main.cpp
 *  @brief the entry point geocoder program
 *  @author Bobrov A.E.
 *  @date 11.09.2016
 */

// std
#include <iostream>
#include <future>
#include <map>
#include <fstream>

// boost
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// this
#include "GeocoderVersion.h"
#include "utils/logger/logger.h"
#include "utils/parse_cmd.h"
#include "geo/geopool.h"
#include "utils/utils.h"

using Answers = std::vector<geocoder::geo::Answer>;
using Addresses = std::vector<std::string>;

Addresses readFromFile(const boost::filesystem::path &filename)
{
  std::ifstream fin{filename.string()};
  
  std::string line;
  Addresses result;

  while (std::getline(fin, line))
  {
    result.emplace_back(line);
  }
  return result;
}

// geocoding addrs
Answers geocoding(const Addresses &addrs, const boost::property_tree::ptree &conf)
{
  auto &logger = geo_logger::get();
  using geocoder::utils::logger::Severity;

  BOOST_LOG_SEV(logger, Severity::info) << "[geocoding]: Start geocode addresses '" << addrs.size() << "'";

  const auto size = addrs.size();
  constexpr auto maxthread = 2;

  // conunt address on thread
  const auto addr_per_thr = size / maxthread;
  auto begin_it = std::begin(addrs);

  std::vector<std::future<Answers>> futures;
  
  for (std::size_t i = 0; i < maxthread; ++i)
  {
    const auto dist = std::distance(begin_it, std::end(addrs));
    const auto d = (dist < addr_per_thr) ? dist : addr_per_thr;
    
    auto it = begin_it;
    std::advance(it, d);
    
    Addresses tmp {begin_it, it};
      
    auto ret = std::async(std::launch::async, [tmp = std::move(tmp), conf]()
        {
          geocoder::geo::GeoPool geo(conf);
          
          Answers result;

          for (const auto &i: tmp)
          {
            try
            {
              auto ret = geo.geocode(i);
              result.push_back(std::move(ret));
            }
            catch (const std::exception &err)
            {
              auto &logger = geo_logger::get();
              BOOST_LOG_SEV(logger, Severity::warning) << "[geocoding]: failed geocode '" << err.what() << "'";
            }
            
          }

          return result;
        });
      begin_it = ++it;
      futures.push_back(std::move(ret));
  }

  // waiting result
  
  Answers result;
  for (auto &i: futures)
  {
    try
    {
      auto ans = i.get();
      result.insert(std::end(result), std::begin(ans), std::end(ans));
    }
    catch (const std::exception &err)
    {
      BOOST_LOG_SEV(logger, Severity::error) << "[geocoding]: failed waiting futures '" << err.what() << "'";
    }
  }

  return result;
}

void print(const boost::filesystem::path &filename, const Answers &answers)
{
  std::ofstream fout(filename.string());

  if (!fout.is_open())
  {
    throw std::runtime_error("[print]: failed open filename '" + filename.string() + "'");
  }

  std::ostringstream out;
  for (const auto &i: answers)
  {
    out << "geocoder type = '" << geocoder::geo::Answer::GeoTypeToText(i.type) << "'\n";

    for (const auto &j: i.locations)
    {
      out << j.line << "\n";
      out << "--> country = '" << j.country << "'\n";
      out << "--> region = '" << j.region << "'\n";
      out << "--> district = '" << j.district << "'\n";
      out << "--> place = '" << j.place << "'\n";
      out << "--> suburb = '" << j.suburb << "'\n";
      out << "--> street = '" << j.street << "'\n";
      out << "--> house = '" << j.house << "'\n";
      out << "--> coord {" << j.coord.latitude << ", " << j.coord.longitude << "}\n";
      out << "--> precision = '" << geocoder::geo::PrecisionToString(j.precision) << "'\n";
    }

    out << "----------------------------------------------------------\n";
    const auto &txt = out.str();
    fout.write(txt.c_str(), txt.length());
    out.str(std::string());
  }
}

int main(int argc, char *argv[])
{
  namespace fs = boost::filesystem;
  
  fs::path config_filename;
  fs::path addr_filename;
  std::string addr;
  fs::path out_filename;

  // parse cmd
  if (!geocoder::utils::parseCmd(argc, argv, config_filename, addr, addr_filename, out_filename))
  {
    std::cerr << "[main]: failed parse command line \n";
    return EXIT_FAILURE;
  }

  if (!fs::exists(out_filename))
  {
    const auto name = geocoder::utils::generateFileName("geocoder", "dat");
    
    out_filename = fs::current_path();
    out_filename /= name;
  }

  // init logger
  geocoder::utils::logger::Logger::initFromFile(config_filename);

  auto &logger = geo_logger::get();
  using geocoder::utils::logger::Severity;

  BOOST_LOG_SEV(logger, Severity::info) << "[main]: Start '" << geocoder::version::getText() << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: config filename = '" << config_filename << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: address filename = '" << addr_filename << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: out filename = '" << out_filename << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: address = '" << addr << "'";
 

  try
  { 
    // read config
    namespace pt = boost::property_tree;
    pt::ptree document;
    pt::read_xml(config_filename.string(), document);
   
    
    if (!addr.empty() && fs::exists(addr_filename))
    {
      BOOST_LOG_SEV(logger, Severity::fatal) << "[main]: Ambiguity parameters -a or -A";
      return EXIT_FAILURE;
    }


    Addresses addrs;
    if (!addr.empty())
    {
      addrs.push_back(addr);
    }
    else if (fs::exists(addr_filename))
    {
      auto addr_from_file = readFromFile(addr_filename);
      std::swap(addrs, addr_from_file);
    }

    auto result = geocoding(addrs, document);
    print(out_filename, result);
  }
  catch (const std::exception &err)
  {
    BOOST_LOG_SEV(logger, geocoder::utils::logger::Severity::fatal) << "Failed '" << err.what() << "'";
    std::cerr << err.what() << std::endl;
  }

  BOOST_LOG_SEV(logger, Severity::info) << "[main]: complete geocoder.";
  return 0;
}

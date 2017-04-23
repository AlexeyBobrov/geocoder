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

Answers geocode(const Addresses &addrs, const boost::property_tree::ptree &conf)
{
  auto &logger = geo_logger::get();
  using geocoder::utils::logger::Severity;
  
  
  BOOST_LOG_SEV(logger, Severity::info) << "[geocode]: Start geocoding.";

  const auto maxthreads = (addrs.size() > 1) ? static_cast<std::size_t>(2) : static_cast<std::size_t>(1);
  const auto addr_per_thread = std::ceil(static_cast<float>(addrs.size()) 
      / static_cast<float>(maxthreads));

  auto start_it = std::cbegin(addrs);

  Answers result;
  std::vector<std::future<Answers>> futures;

  for (std::size_t i = 0; i < maxthreads; ++i)
  {
    const auto dist = std::distance(start_it, std::cend(addrs));
    if (dist <= 0)
    {
      break;
    }

    const auto count_addrs = (dist > addr_per_thread) ? addr_per_thread : dist;
    auto begin_it = start_it;
    std::advance(start_it, count_addrs);

    Addresses tmp{begin_it, start_it};
    
    auto fut = std::async(std::launch::async, [tmp = std::move(tmp), conf]
        {
          auto &logger = geo_logger::get();
          geocoder::geo::GeoPool pool(conf);
          Answers answers;
          for (const auto &i: tmp)
          {
            try
            { 
              auto answer = pool.geocode(i);
              answers.push_back(answer);
            }
            catch (const std::exception& err)
            {
              BOOST_LOG_SEV(logger, Severity::error) << "[geocode] Failed geocode '" << i << "'"; 
            }
          }

          return answers;
        });
    
    futures.push_back(std::move(fut));
  }
  
  for (auto &i: futures)
  {
    auto answers = i.get();
    result.insert(std::cend(result), std::cbegin(answers), std::cend(answers));
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
    return 0;
  }

  const auto config = geocoder::utils::getRealFileName(config_filename);
  const auto file_result = geocoder::utils::getRealFileName(out_filename);

  // init logger
  geocoder::utils::logger::Logger::initFromFile(config);

  auto &logger = geo_logger::get();
  using geocoder::utils::logger::Severity;

  BOOST_LOG_SEV(logger, Severity::info) << "[main]: Start '" << geocoder::version::getText() << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: config filename = '" << config << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: address filename = '" << addr_filename << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: out filename = '" << file_result << "'";
  BOOST_LOG_SEV(logger, Severity::info) << "[main]: address = '" << addr << "'";

  try
  { 
    // read config
    namespace pt = boost::property_tree;
    pt::ptree document;
    pt::read_xml(config.string(), document);
   
    if (!addr.empty() && fs::exists(addr_filename))
    {
      BOOST_LOG_SEV(logger, Severity::fatal) << "[main]: Ambiguity parameters -a or -A";
      return EXIT_FAILURE;
    }

    Addresses addrs;
    if (!addr.empty())
    {
      BOOST_LOG_SEV(logger, Severity::debug) << "[main]: address '" << addr << "'";
      addrs.push_back(addr);
    }
    else if (fs::exists(addr_filename))
    {
      auto addr_from_file = readFromFile(addr_filename);
      std::swap(addrs, addr_from_file);
    }

    auto result = geocode(addrs, document);
    print(file_result, result);
  }
  catch (const std::exception &err)
  {
    BOOST_LOG_SEV(logger, geocoder::utils::logger::Severity::fatal) << "Failed '" << err.what() << "'";
    std::cerr << err.what() << std::endl;
  }

  BOOST_LOG_SEV(logger, Severity::info) << "[main]: complete geocoder.";
  boost::log::core::get()->remove_all_sinks();
  
  return 0;
}

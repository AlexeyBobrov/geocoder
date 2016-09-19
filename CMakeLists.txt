cmake_minimum_required (VERSION 2.8)

include(env.cmake)
include(version.cmake)

project (${ProjectName})

find_package(Boost 1.55 COMPONENTS log_setup log system thread date_time filesystem program_options REQUIRED)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME ON)
set (Boost_USE_MULTITHREADED ON)

add_definitions(-DBOOST_LOG_DYN_LINK)

configure_file(
  "${PROJECT_SOURCE_DIR}/inc/GeocoderVersion.h.in"
  "${PROJECT_BINARY_DIR}/GeocoderVersion.h"
  )

include_directories(${PROJECT_BINARY_DIR}
  inc
  ${Boost_INCLUDE_DIRS}
  /usr/include
  )


set (EXECUTABLE_OUTPUT_PATH "../bin")

set (INCLUDES
  inc
  ${Boost_INCLUDE_DIRS}
  /usr/include)

set (SOURCES 
  src/main.cpp
  src/utils/libcurl/libcurl.cpp
  src/utils/logger/config.cpp
  src/utils/logger/logger.cpp
  src/geo/geocoderbase.cpp
  src/geo/geoyandex.cpp
  src/geo/geopool.cpp
  src/utils/parse_cmd.cpp
  src/utils/utils.cpp
  )

set (LIBRARIES
  pthread
  curl
  ${Boost_LIBRARIES}
  )
set (CMAKE_CXX_FLAGS "-std=c++1y -Wall")
add_executable(${ProjectName} ${INCLUDES} ${SOURCES} src/main.cpp)
target_link_libraries(${ProjectName} ${LIBRARIES})




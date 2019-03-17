cmake_minimum_required (VERSION 2.8)

include(env.cmake)
include(version.cmake)

project (${ProjectName})

set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_STATIC_RUNTIME OFF)
set (Boost_USE_MULTITHREADED ON)
#set (Boost_NO_SYSTEM_PATHS ON)
#set (BOOST_INCLUDEDIR "/home/alexey/3rdparty/boost/install/include")
#set (BOOST_LIBRARYDIR "/home/alexey/3rdparty/boost/install/lib")
#set (BOOST_INCLUDEDIR "/opt/local/include")
#set (BOOST_LIBRARYDIR "/opt/local/lib")
#set (BOOST_INCLUDEDIR "/opt/local/include")
#set (BOOST_LIBRARYDIR "/opt/local/lib/boost")
#set (BOOST_LIBRARYDIR "/opt/local/lib64")

add_definitions(-DBOOST_LOG_DYN_LINK)
add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DBOOST_TEST_DYN_LINK)
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS)


find_package(Boost 1.55 COMPONENTS log_setup log system thread date_time filesystem program_options unit_test_framework REQUIRED)

configure_file(
  "${PROJECT_SOURCE_DIR}/inc/GeocoderVersion.h.in"
  "${PROJECT_BINARY_DIR}/GeocoderVersion.h"
  )

include_directories(${PROJECT_BINARY_DIR}
  inc
  test
  ${Boost_INCLUDE_DIRS}
  )

set (EXECUTABLE_OUTPUT_PATH "../bin")

set (INCLUDES
  inc
  test
  ${Boost_INCLUDE_DIRS})

set (SOURCES 
  src/utils/libcurl/libcurl.cpp
  src/utils/logger/config.cpp
  src/utils/logger/logger.cpp
  src/geo/geocoderbase.cpp
  src/geo/geoyandex.cpp
  src/geo/geopool.cpp
  src/utils/parse_cmd.cpp
  src/utils/utils.cpp
  )

set (SOURCES_TEST 
  ${SOURCES}
  test/test_geocoder.cpp
  test/test_utils.cpp
  )

set (LIBRARIES
  pthread
  curl
  ${Boost_LIBRARIES}
  )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (CMAKE_CXX_FLAGS "-std=c++1y -stdlib=libc++ -Wall -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "-std=c++14 -Wall")
else()
  message(FATAL_ERROR "Unknown compiler, compiler id = ${CMAKE_CXX_COMPILER_ID}")
endif()

add_executable(${ProjectName} ${INCLUDES} ${SOURCES} src/main.cpp)
target_link_libraries(${ProjectName} ${LIBRARIES})

add_executable(${ProjectName}_test ${INCLUDES} ${SOURCES_TEST} test/test_main.cpp)
target_link_libraries(${ProjectName}_test ${LIBRARIES})


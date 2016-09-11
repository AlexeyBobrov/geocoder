cmake_minimum_required (VERSION 2.8)

include(env.cmake)
include(version.cmake)

project (${ProjectName})

configure_file(
  "${PROJECT_SOURCE_DIR}/inc/GeocoderVersion.h.in"
  "${PROJECT_BINARY_DIR}/GeocoderVersion.h"
  )

include_directories(${PROJECT_BINARY_DIR}
  inc)

set (EXECUTABLE_OUTPUT_PATH "../bin")

set (SOURCES 
  src/main.cpp)

add_executable(${ProjectName} ${SOURCES} src/main.cpp)



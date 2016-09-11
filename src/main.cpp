/** @file main.cpp
 *  @brief the entry point geocoder program
 *  @author Bobrov A.E.
 *  @date 11.09.2016
 */

// std
#include <iostream>

// this
#include "GeocoderVersion.h"

int main(int argc, char *argv[])
{
  std::cout << geocoder::version::getText() << std::endl;
  return 0;
}

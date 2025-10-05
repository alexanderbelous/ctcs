cmake_minimum_required(VERSION 4.0)

# Requires C++20.
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ctcs VERSION 1.0.0 LANGUAGES CXX)

add_library(ctcs INTERFACE
  "include/ctcs/internal/ArithmeticCoder.h"
  "include/ctcs/internal/ArithmeticCoding.h"
  "include/ctcs/internal/ArithmeticCodingCommon.h"
  "include/ctcs/internal/ArithmeticDecoder.h"
  "include/ctcs/internal/IBitStream.h"
  "include/ctcs/internal/OBitStream.h"
  "include/ctcs/internal/VarInt.h"
  "include/ctcs/ArithmeticCodingCompressor.h"
  "include/ctcs/CompressedString.h"
  "include/ctcs/EnglishCharModel.h"
  "include/ctcs/StringLiteral.h"
  "include/ctcs/ctcs.h"
)
add_library(ctcs::ctcs ALIAS ctcs)

target_include_directories(ctcs INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

add_subdirectory(tests)

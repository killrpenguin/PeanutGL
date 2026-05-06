cmake_minimum_required(VERSION 3.20...4.10)

project(
  PeanutGL
  VERSION 1.0
  DESCRIPTION "An adoption of the Chernos Walnut App in C++26 and CMake."
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  add_subdirectory(Tests)
  include(CTest)
endif()

# Global Settings

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_COMPILER "/usr/lib/llvm/22/bin/clang")

set(CMAKE_CXX_COMPILER "/usr/lib/llvm/22/bin/clang++")
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--allow-no-checks;-checks=")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# User-configurable options
option(BUILD_EXAMPLES "Build example applications" OFF)
option(BUILD_STATIC_LIB "Build static library only" OFF)
option(ENABLE_TIME_TRACE "Add -ftime-trace to Clang builds" OFF)
option(ENABLE_DOCS "Build with engine documentation." OFF)

include(CMake/Dependencies.cmake)
include(CMake/GeneratedConstants.cmake)
include(CMake/LibunwindExtern.cmake)
# The compiled library code is here
add_subdirectory(PeanutGL)

# The executable code is here
add_subdirectory(Examples)

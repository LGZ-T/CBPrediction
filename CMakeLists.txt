cmake_minimum_required(VERSION 2.8)
project(CBPrediction)

set(CMAKE_MODULE_PATH 
   ${CMAKE_MODULE_PATH}
   ${CMAKE_SOURCE_DIR}/cmake
   )
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
endif()

find_package(LLVM)

option(DYNAMIC_LINK "Link Dynamic LLVM Libraries" ${LLVM_DYNAMIC_LIBRARY_FOUND})
option(WITH_DEBUG "Enable Extra Debug Output" On)


find_package(PkgConfig)
pkg_check_modules(GSL gsl REQUIRED)

if(NOT WITH_DEBUG)
   set(NO_DEBUG On)
endif()

add_subdirectory(src)

 cmake_minimum_required(VERSION 3.22.1)
 set(CMAKE_CXX_STANDARD 20)

 project("simplygraph")

 file(
         DOWNLOAD
         https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.1/CPM.cmake
         ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
         EXPECTED_HASH SHA256=f3a6dcc6a04ce9e7f51a127307fa4f699fb2bade357a8eb4c5b45df76e1dc6a5
 )

 include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

 CPMAddPackage(
         NAME expertk
         GITHUB_REPOSITORY ArashPartow/exprtk
         GIT_TAG cmake
 )

 include_directories(include)
 include_directories(${expertk_SOURCE_DIR}/include)

 add_library(
         native-lib
         SHARED
         native-lib.cpp
 )

 find_library(
         log-lib
         log)

 target_link_libraries(
         native-lib
         ${log-lib})
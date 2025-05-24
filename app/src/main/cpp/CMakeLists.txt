 cmake_minimum_required(VERSION 3.22.1)

 project("simplygraph")

 file(
         DOWNLOAD
         https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
         ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
         EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
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
         funcs.cpp)

 find_library(
         log-lib
         log)

 target_link_libraries(
         native-lib
         ${log-lib})
 cmake_minimum_required(VERSION 3.22.1)

 project("simplygraph")

 include_directories(include)
 add_subdirectory(exprtk)

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
         exprtk
         ${log-lib})
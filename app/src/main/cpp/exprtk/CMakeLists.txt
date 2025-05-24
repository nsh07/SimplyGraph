#
# **************************************************************
# *         C++ Mathematical Expression Toolkit Library        *
# *                                                            *
# * Author: Arash Partow (1999-2024)                           *
# * URL: https://www.partow.net/programming/exprtk/index.html  *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of the Mathematical Expression Toolkit Library is *
# * permitted under the guidelines and in accordance with the  *
# * most current version of the MIT License.                   *
# * https://www.opensource.org/licenses/MIT                    *
# * SPDX-License-Identifier: MIT                               *
# *                                                            *
# **************************************************************
#

cmake_minimum_required(VERSION 3.15)

project(
    ExprTk
    LANGUAGES CXX
    VERSION 0.0.1
    DESCRIPTION  "C++ Mathematical Expression Toolkit Library (ExprTk)"
    HOMEPAGE_URL "https://www.partow.net/programming/exprtk/index.html"
)

# define ExprTk header-only library
add_library(
    exprtk
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/exprtk.hpp
)

add_library(exprtk::exprtk ALIAS exprtk)

target_include_directories(
    exprtk
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(exprtk INTERFACE cxx_std_11)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "ExprTkConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# If top level then build everything
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)

   if (NOT CMAKE_BUILD_TYPE)
       message(STATUS "Build has type not been set - defaulting to Debug")
       set(CMAKE_BUILD_TYPE "Debug")
   endif()

   set(CMAKE_CXX_STANDARD            20 )
   set(CMAKE_CXX_STANDARD_REQUIRED   ON )
   set(CMAKE_CXX_EXTENSIONS          OFF)
   set(CMAKE_VERBOSE_MAKEFILE        ON )
   set(CMAKE_EXPORT_COMPILE_COMMANDS ON )

   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_SOURCE_DIR}/build                    )
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/bin)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/bin  )

   if (MSVC)

      add_definitions(/bigobj)

      add_compile_options(/W4)
      add_compile_options(/WX)

      add_compile_options(/wd4334)
      add_compile_options(/wd4702)

      add_compile_definitions(_SECURE_SCL=0            )
      add_compile_definitions(_CRT_SECURE_NO_WARNINGS  )
      add_compile_definitions(_SCL_SECURE_NO_WARNINGS  )
      add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0)

   else()

      add_compile_options( -pedantic-errors )
      add_compile_options( -Wall            )
      add_compile_options( -Wextra          )
      add_compile_options( -Werror          )

      if ((ENABLE_SANITIZERS) AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
         message("Enabling sanitizers: ASAN, LSAN, USAN")

         add_compile_options( -fsanitize=address      )
         add_compile_options( -fsanitize=leak         )
         add_compile_options( -fsanitize=undefined    )
         add_compile_options( -fno-omit-frame-pointer )

         add_link_options   ( -fsanitize=address      )
         add_link_options   ( -fsanitize=leak         )
         add_link_options   ( -fsanitize=undefined    )
         add_link_options   ( -fno-omit-frame-pointer )

         set(CMAKE_EXE_LINKER_FLAGS -fuse-ld=gold)
      endif()

      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

   endif()

   add_subdirectory(src)

endif()

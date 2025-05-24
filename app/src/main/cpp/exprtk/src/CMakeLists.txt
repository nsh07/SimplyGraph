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


file(GLOB files "exprtk_*.cpp")

foreach(sourcefile ${files})
   get_filename_component(app_name ${sourcefile} NAME_WE)
   add_executable(${app_name} ${sourcefile})
   target_link_libraries(${app_name} PRIVATE exprtk::exprtk)
   set_target_properties(${app_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endforeach(sourcefile ${files})

/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Pyramid                                             *
 * Author: Arash Partow (1999-2024)                           *
 * URL: https://www.partow.net/programming/exprtk/index.html  *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the MIT License.                   *
 * https://www.opensource.org/licenses/MIT                    *
 * SPDX-License-Identifier: MIT                               *
 *                                                            *
 **************************************************************
*/


#include <cstdio>
#include <string>

#include "exprtk.hpp"


template <typename T>
void pyramid()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;

   symbol_table.add_package(io_package);

   const std::string pyramid_program =
      " const var n := 30;                           "
      "                                              "
      " for (var i := 0; i < n; i += 1)              "
      " {                                            "
      "    for (var j := 0; j < n - i - 1; j += 1)   "
      "    {                                         "
      "       print(' ');                            "
      "    };                                        "
      "                                              "
      "    for (var j := 0; j < 2 * i + 1; j += 1)   "
      "    {                                         "
      "       print('*');                            "
      "    };                                        "
      "                                              "
      "    println();                                "
      " }                                            ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(pyramid_program,expression);

   expression.value();
}

int main()
{
   pyramid<double>();
   return 0;
}

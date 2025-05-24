/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Archimedes Approximation Of Pi                      *
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
void archimedes_pi()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   exprtk::rtl::io::println<T> println("%25.23f");

   symbol_table_t symbol_table;

   symbol_table.add_function("println", println);
   symbol_table.add_constants();

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("archimedes_pi")
      .var("iterations")
      .expression
      (
         " var s         := 1.0;                    "
         " var approx_pi := 0.0;                    "
         "                                          "
         " for (var i := 0; i < iterations; i += 1) "
         " {                                        "
         "     var n  := 6 * 2^i;                   "
         "     var s2 := (s / 2)^2;                 "
         "     var t  := sqrt(1 - s2);              "
         "     var q  := 1 - t;                     "
         "     var p  := n * s;                     "
         "                                          "
         "     approx_pi := p / 2;                  "
         "     s         := sqrt(q^2 + s2);         "
         " };                                       "
         "                                          "
         " approx_pi;                               "
      ));

   const std::string archimedes_pi_program =
      " const var n := 30;                                              "
      "                                                                 "
      " for (var i := 1; i <= n; i += 1)                                "
      " {                                                               "
      "    var approx_pi := archimedes_pi(i);                           "
      "    var abs_error := abs(approx_pi - pi);                        "
      "                                                                 "
      "    println('approx pi: ', approx_pi, ' abs error:', abs_error); "
      " }                                                               ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(archimedes_pi_program,expression);

   expression.value();
}

int main()
{
   archimedes_pi<double>();
   return 0;
}

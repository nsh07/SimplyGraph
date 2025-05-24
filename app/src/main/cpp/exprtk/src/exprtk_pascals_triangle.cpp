/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Pascal's Triangle                                   *
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
void pascals_triangle()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;
   symbol_table.add_package(io_package);
   symbol_table.add_function("print_coeff",
                             [](T t)
                             {
                                printf("%6d", static_cast<unsigned int>(t));
                                return T(0);
                             });

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("n_choose_k")
      .vars("n","k")
      .expression
      (
        " switch                                      "
        " {                                           "
        "    case n <= k : 1;                         "
        "    case k <= 0 : 1;                         "
        "    default     : n_choose_k(n - 1, k - 1) + "
        "                  n_choose_k(n - 1, k    ) ; "
        " }                                           "
      ));

   const std::string pascals_triangle_program =
      " const var rows := 20;                            "
      "                                                  "
      " for (var n := 0; n < rows; n += 1)               "
      " {                                                "
      "    for (var s := 0; s <= (rows - n) - 2; s += 1) "
      "    {                                             "
      "       print('   ');                              "
      "    };                                            "
      "                                                  "
      "    for (var k := 0; k <= n; k += 1)              "
      "    {                                             "
      "       print_coeff(n_choose_k(n,k));              "
      "    };                                            "
      "                                                  "
      "    println();                                    "
      " }                                                ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(pascals_triangle_program,expression);

   expression.value();
}

int main()
{
   pascals_triangle<double>();
   return 0;
}

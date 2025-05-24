/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Binomial Coefficient (n-choose-r)                   *
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
inline T ncr_println(T n, T r, T ncr)
{
   printf("ncr(%2d,%2d) = %2llu\n",
          static_cast<int>(n),
          static_cast<int>(r),
          static_cast<unsigned long long>(ncr));

   return T(0);
}

template <typename T>
void n_choose_r()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   symbol_table_t symbol_table;
   symbol_table.add_function("println",ncr_println);

   compositor_t compositor(symbol_table);

   // define function: ncr(n,r)
   compositor.add(
      function_t("ncr")
      .vars("n", "r")
      .expression
      (
        " switch                               "
        " {                                    "
        "    case n <= r : 1;                  "
        "    case r <= 0 : 1;                  "
        "    default     : ncr(n - 1, r - 1) + "
        "                  ncr(n - 1, r    ) ; "
        " }                                    "
      ));

   const std::string ncr_program =
      " var n := 25;                     "
      " for (var r := 1; r < n; r += 1)  "
      " {                                "
      "    println(n, r, ncr(n,r));      "
      " };                               ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(ncr_program,expression);

   expression.value();
}

int main()
{
   n_choose_r<double>();
   return 0;
}

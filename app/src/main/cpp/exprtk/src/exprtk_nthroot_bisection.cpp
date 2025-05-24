/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Nth-Root via Bisection Method                       *
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


#include <cmath>
#include <cstdio>
#include <string>

#include "exprtk.hpp"


template <typename T>
void nthroot_via_bisection()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   const std::string nthroot_via_bisection_program =
      " for (var x := -30; x <= 30; x += 1)   "
      " {                                     "
      "    println('[', x, ']',               "
      "            ' sqrt = ', nthroot(x,2),  "
      "            ' cbrt = ', nthroot(x,3)); "
      " }                                     ";

   exprtk::rtl::io::println<T> println;

   symbol_table_t symbol_table;
   symbol_table.add_function("println",println);

   compositor_t compositor(symbol_table);

   // define function: nthroot(x,n)
   compositor.add(
      function_t("nthroot")
      .var("x")
      .var("n")
      .expression
      (
        " if (abs(frac(n)) > 0)            "
        "    return [null];                "
        " else if (x < 0 and (n % 2 == 0)) "
        "    return [null];                "
        "                                  "
        " var lo := min(0,x);              "
        " var hi := max(0,x);              "
        "                                  "
        " while (true)                     "
        " {                                "
        "    var mid := avg(lo,hi);        "
        "    var y   := pow(mid,n);        "
        "                                  "
        "    if (equal(y, x))              "
        "       break [mid];               "
        "    else if (y < x)               "
        "       lo := mid;                 "
        "    else                          "
        "       hi := mid;                 "
        " };                               "
       ));

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(nthroot_via_bisection_program, expression);

   expression.value();
}

int main()
{
   nthroot_via_bisection<double>();
   return 0;
}

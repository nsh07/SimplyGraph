/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Summation of Prime Factors                          *
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
void sum_of_primes()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   symbol_table_t symbol_table;

   compositor_t compositor(symbol_table);

   // define function: sum_of_primes(z)
   compositor.add(
      function_t("sum_of_primes")
        .var("z")
        .expression(
          " var i     := 2;      "
          " var total := 0;      "
          " while (z > 1)        "
          " {                    "
          "    if (0 == (z % i)) "
          "    {                 "
          "       total += i;    "
          "       z /= i;        "
          "    }                 "
          "    else              "
          "       i += 1;        "
          " };                   "
          " total;               "));

   exprtk::rtl::io::println<T> println;

   symbol_table.add_function("println",println);

   const std::string sum_of_prime_factors_program =
      " for (var i := 1; i <= 100; i += 1) "
      " {                                  "
      "    println(i, sum_of_primes(i));   "
      " }                                  ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(sum_of_prime_factors_program,expression);

   expression.value();
}

int main()
{
   sum_of_primes<double>();
   return 0;
}

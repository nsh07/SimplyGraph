/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Primes Via The Naive Method                         *
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
void primes_via_naive_method()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   compositor_t compositor;

   compositor.add(
      function_t("is_prime")
      .var("x")
      .expression
      (
         " switch                                                    "
         " {                                                         "
         "    case x <= 1       : return [false];                    "
         "    case frac(x) != 0 : return [false];                    "
         "    case x == 2       : return [true ];                    "
         "    default           :                                    "
         "    {                                                      "
         "       var prime_lut[81] :=                                "
         "       {                                                   "
         "            2,   3,   5,   7,  11,  13,  17,  19,  23,     "
         "           29,  31,  37,  41,  43,  47,  53,  59,  61,     "
         "           67,  71,  73,  79,  83,  89,  97, 101, 103,     "
         "          107, 109, 113, 127, 131, 137, 139, 149, 151,     "
         "          157, 163, 167, 173, 179, 181, 191, 193, 197,     "
         "          199, 211, 223, 227, 229, 233, 239, 241, 251,     "
         "          257, 263, 269, 271, 277, 281, 283, 293, 307,     "
         "          311, 313, 317, 331, 337, 347, 349, 353, 359,     "
         "          367, 373, 379, 383, 389, 397, 401, 409, 419      "
         "       };                                                  "
         "                                                           "
         "       var upper_bound := min(x - 1, trunc(sqrt(x)) + 1);  "
         "                                                           "
         "       for (var i := 0; i < prime_lut[]; i += 1)           "
         "       {                                                   "
         "          if (prime_lut[i] >= upper_bound)                 "
         "             return [true];                                "
         "          else if ((x % prime_lut[i]) == 0)                "
         "             return [false];                               "
         "       };                                                  "
         "                                                           "
         "       var lower_bound := prime_lut[prime_lut[] - 1] + 2;  "
         "                                                           "
         "       for (var i := lower_bound; i < upper_bound; i += 2) "
         "       {                                                   "
         "          if ((x % i) == 0)                                "
         "          {                                                "
         "             return [false];                               "
         "          }                                                "
         "       }                                                   "
         "    };                                                     "
         " };                                                        "
         "                                                           "
         " return [true];                                            "
      ));

   const std::string primes_via_naive_method_program =
      " for (var i := 1; i < 10000; i += 1) "
      " {                                   "
      "    if (is_prime(i))                 "
      "    {                                "
      "       println(i, ' is prime');      "
      "    }                                "
      " };                                  ";

   exprtk::rtl::io::println<T> println;

   symbol_table_t& symbol_table = compositor.symbol_table();
   symbol_table.add_function("println",println);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(primes_via_naive_method_program,expression);

   expression.value();
}

int main()
{
   primes_via_naive_method<double>();
   return 0;
}

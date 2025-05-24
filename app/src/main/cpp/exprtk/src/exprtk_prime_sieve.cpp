/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Sieve Of Eratosthenes                               *
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
void sieve_of_eratosthenes()
{
   typedef exprtk::expression<T> expression_t;
   typedef exprtk::parser<T>     parser_t;

   const std::string sieve_of_eratosthenes_program =
      " var sieve[10^8] := [false];                   "
      " var m := trunc(sqrt(sieve[]));                "
      "                                               "
      " sieve[0] := true;                             "
      " sieve[1] := true;                             "
      "                                               "
      " for (var i := 0; i <= m; i += 1)              "
      " {                                             "
      "    if (false == sieve[i])                     "
      "    {                                          "
      "       for (var j := i^2; j < sieve[]; j += i) "
      "       {                                       "
      "          sieve[j] := true;                    "
      "       }                                       "
      "    }                                          "
      " };                                            "
      "                                               "
      " var prime_count := sieve[] - sum(sieve);      "
      "                                               "
      " prime_count == 5761455;                       ";

   expression_t expression;

   parser_t parser;
   parser.compile(sieve_of_eratosthenes_program,expression);

   exprtk::timer timer;
   timer.start();

   const T result = expression.value();

   timer.stop();

   printf("Result: %8.3f\n",result);

   printf("Total time: %8.3fsec\n",timer.time());
}

int main()
{
   sieve_of_eratosthenes<double>();
   return 0;
}

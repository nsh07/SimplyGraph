/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Approximation of e via Monte-Carlo Method                  *
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
#include <cstdlib>
#include <ctime>
#include <string>

#include "exprtk.hpp"


template <typename T>
struct rnd_01 : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   rnd_01() : exprtk::ifunction<T>(0)
   { ::srand(static_cast<unsigned int>(time(NULL))); }

   inline T operator()()
   {
      // Note: Do not use this in production
      // Result is in the interval [0,1)
      return T(::rand() / T(RAND_MAX + 1.0));
   }
};

template <typename T>
void monte_carlo_e()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string monte_carlo_e_program =
      " var max_samples := 10^8;                  "
      " var trials      := 0;                     "
      "                                           "
      " for (var i := 0; i < max_samples; i += 1) "
      " {                                         "
      "    var rand_sum := 0;                     "
      "    repeat                                 "
      "       rand_sum += rnd_01;                 "
      "       trials   += 1;                      "
      "    until (rand_sum > 1);                  "
      " };                                        "
      "                                           "
      " trials / max_samples;                     ";

   rnd_01<T> rnd01;

   symbol_table_t symbol_table;
   symbol_table.add_function("rnd_01",rnd01);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(monte_carlo_e_program,expression);

   const T approximate_e = expression.value();

   const T real_e = T(2.718281828459045235360287471352662); // or close enough...

   printf("e ~ %20.17f\terror: %20.17f\n",
          approximate_e,
          std::abs(real_e - approximate_e));
}

int main()
{
   monte_carlo_e<double>();
   return 0;
}

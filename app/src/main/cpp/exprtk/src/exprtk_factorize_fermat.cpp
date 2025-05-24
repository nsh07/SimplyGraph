/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Fermat's Factorization Example                      *
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
void factorize_fermats_method()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   // Form: n = p*q where p,q are prime
   T composites[] =
   {
       199203677,  779234623,  843093203,  883543291, 1197162971, 1282615157, 1368190717,
      1552390397, 1765737859, 1800091571, 1878769589, 1993904873, 2257133471, 2520523529,
      2579094799, 2853450949, 2935025369, 3095780533, 3164132249, 3408963511, 4260042859,
      4608613981, 4654875857, 5085931997, 6064982771, 7278175081, 7289187463, 9206112101
   };

   exprtk::rtl::io::println<T> println;

   symbol_table_t symbol_table;

   symbol_table.add_vector  ("composites", composites);
   symbol_table.add_function("println"   , println   );

   compositor_t compositor(symbol_table);

   compositor.add(function_t()
      .name("is_square")
      .var("x")
      .expression
      (
         " var s := sqrt(x);  "
         " var t := trunc(s); "
         " (s - t) == 0;      "
      ));

   compositor.add(function_t()
      .name("fermat")
      .var("n")
      .expression
      (
         " if (n == 0)                     "
         " {                               "
         "    return [0];                  "
         " }                               "
         " else if ((n % 2) == 0)          "
         " {                               "
         "    return [n / 2];              "
         " };                              "
         "                                 "
         " var a := ceil(sqrt(n));         "
         "                                 "
         " while (not(is_square(a^2 - n))) "
         " {                               "
         "    a += 1;                      "
         " };                              "
         "                                 "
         " var b := sqrt(a^2 - n);         "
         "                                 "
         " a - b;                          "
      ));

   const std::string factorize_composites_program =
      " for (var i := 0; i < composites[]; i += 1)                               "
      " {                                                                        "
      "    var n       := composites[i];                                         "
      "    var factor0 := fermat(n);                                             "
      "    var factor1 := n / factor0;                                           "
      "                                                                          "
      "    if ((factor0 * factor1 == n) and (factor0 != 1))                      "
      "    {                                                                     "
      "       println('n: ', n, ' factors: { ', factor0 ,' , ', factor1 ,' } '); "
      "    }                                                                     "
      "    else                                                                  "
      "    {                                                                     "
      "       println('failed to factorize ', n);                                "
      "    }                                                                     "
      " }                                                                        "
      "                                                                          ";
   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(factorize_composites_program,expression);

   expression.value();
}

int main()
{
   factorize_fermats_method<double>();
   return 0;
}

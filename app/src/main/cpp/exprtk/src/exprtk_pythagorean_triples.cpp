/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Pythagorean Triples                                 *
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
void exprtk_pythagorean_triples()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string pythagorean_triples_program =
      " const var num_triples := 1000;                "
      " var triple_count      := 0;                   "
      " var n                 := 2;                   "
      "                                               "
      " while (true)                                  "
      " {                                             "
      "    for (var i := 1; i < n; i += 1)            "
      "    {                                          "
      "       var a := n^2 - i^2;                     "
      "       var b := 2 * n * i;                     "
      "       var c := n^2 + i^2;                     "
      "                                               "
      "       if (a > b) a <=> b;                     "
      "       if (b > c) b <=> c;                     "
      "       if (a > b) a <=> b;                     "
      "                                               "
      "       println('(', a ,',', b ,',', c ,')');   "
      "                                               "
      "       if ((triple_count += 1) >= num_triples) "
      "       {                                       "
      "          return [];                           "
      "       };                                      "
      "    };                                         "
      "                                               "
      "    n += 1;                                    "
      " }                                             ";

   exprtk::rtl::io::println<T> println;

   symbol_table_t symbol_table;
   symbol_table.add_function ("println", println);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(pythagorean_triples_program, expression);

   expression.value();
}

int main()
{
   exprtk_pythagorean_triples<double>();
   return 0;
}

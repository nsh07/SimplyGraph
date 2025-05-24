/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Ten Thousand Digits Of Pi via Spigot Algorithm      *
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
void pi_10k_digits()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   symbol_table_t symbol_table;

   symbol_table.add_function("print",
                             [](T t) -> T
                             {
                                printf("%1d",static_cast<int>(t));
                                return T(1);
                             });

   const std::string pi_10k_digits_spigot =
      " const var num_digits := 10000;                "
      " const var offset     := 14;                   "
      " var a[3.5 * num_digits + offset];             "
      " var size   := a[] - offset;                   "
      " var d      := 0;                              "
      " var c      := 0;                              "
      " var h      := 0;                              "
      " var f      := size / 3.5;                     "
      "                                               "
      " for (var x := 0; x < size ; x += offset)      "
      " {                                             "
      "    var b := size - x - 1;                     "
      "    var g := 2 * b;                            "
      "                                               "
      "    d %= f;                                    "
      "    c := d;                                    "
      "                                               "
      "    while (g > 0)                              "
      "    {                                          "
      "       var k := floor(b);                      "
      "       var i := (h > 0) ? a[k] : floor(f / 5); "
      "       d := (d * b) + (f * i);                 "
      "       g -= 1;                                 "
      "       b -= 1;                                 "
      "       a[k] := d % g;                          "
      "       d := floor(d / g);                      "
      "       g := 2 * b;                             "
      "    };                                         "
      "                                               "
      "    var v := c + floor((1 * d) / f);           "
      "                                               "
      "    for (var j := 1000; j >= 1; j := j / 10)   "
      "    {                                          "
      "       print(floor(v / j));                    "
      "       v %= j;                                 "
      "    };                                         "
      "                                               "
      "    h := 1;                                    "
      " }                                             ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(pi_10k_digits_spigot,expression);

   expression.value();
}

int main()
{
   pi_10k_digits<double>();
   return 0;
}

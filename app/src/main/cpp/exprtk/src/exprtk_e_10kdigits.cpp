/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Ten Thousand Digits Of E via Spigot Algorithm       *
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
void e_10k_digits()
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

   const std::string e_10k_digits_spigot =
      " const var num_digits := 10000;                       "
      " var a[num_digits] := [2:1];                          "
      " var b[num_digits] := [1];                            "
      "                                                      "
      " print(2);                                            "
      "                                                      "
      " for (var digit := 0; digit < num_digits; digit += 1) "
      " {                                                    "
      "    b *= 10;                                          "
      "                                                      "
      "    for (var i := (b[] - 1); i > 0; i -= 1)           "
      "    {                                                 "
      "       var q := floor(b[i] / a[i]);                   "
      "       var r := floor(b[i] % a[i]);                   "
      "       b[i - 1] := b[i - 1] + q;                      "
      "       b[i] := r;                                     "
      "    };                                                "
      "                                                      "
      "    print(floor(b[0] / a[0]));                        "
      "                                                      "
      "    b[0] %= a[0];                                     "
      " }                                                    ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(e_10k_digits_spigot,expression);

   expression.value();
}

int main()
{
   e_10k_digits<double>();
   return 0;
}

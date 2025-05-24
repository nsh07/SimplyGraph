/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Mandelbrot Fractal Generator Example                *
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
inline T putch(T v)
{
   printf("%c",static_cast<int>(v));
   return T(0);
}

template <typename T>
void mandelbrot()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string mandelbrot_program =
      " width    := 118;                                  "
      " height   := 41;                                   "
      " imag_max := +1;                                   "
      " imag_min := -1;                                   "
      " real_max := +1;                                   "
      " real_min := -2.5;                                 "
      " x_step   := (real_max - real_min) / width;        "
      " y_step   := (imag_max - imag_min) / height;       "
      "                                                   "
      " for (var y := 0; y < height; y += 1)              "
      " {                                                 "
      "    imag := imag_min + (y_step * y);               "
      "                                                   "
      "    for (var x := 0; x < width; x += 1)            "
      "    {                                              "
      "       real   := real_min + x_step * x;            "
      "       z_real := real;                             "
      "       z_imag := imag;                             "
      "                                                   "
      "       for (var n := 0; n < 30; n += 1)            "
      "       {                                           "
      "          a := z_real^2;                           "
      "          b := z_imag^2;                           "
      "          plot_value := n;                         "
      "                                                   "
      "          if ((a + b) < 4)                         "
      "          {                                        "
      "             z_imag := 2 * z_real * z_imag + imag; "
      "             z_real := a - b + real;               "
      "          }                                        "
      "          else                                     "
      "             break;                                "
      "       };                                          "
      "                                                   "
      "       putch(61 - plot_value);                     "
      "    };                                             "
      "                                                   "
      "    println()                                      "
      " }                                                 ";

   exprtk::rtl::io::println<T> println;

   symbol_table_t symbol_table;
   symbol_table.add_function("putch"  , putch  );
   symbol_table.add_function("println", println);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.enable_unknown_symbol_resolver();
   parser.compile(mandelbrot_program,expression);

   expression.value();
}

int main()
{
   mandelbrot<double>();
   return 0;
}

/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Fizz-Buzz Example                                   *
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
void fizzbuzz()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string fizzbuzz_program =
      " var x := 1;                              "
      " repeat                                   "
      "   var div3 := ((x % 3) == 0);            "
      "   var div5 := ((x % 5) == 0);            "
      "   [*]                                    "
      "   {                                      "
      "      case div3          : print('fizz'); "
      "      case div5          : print('buzz'); "
      "      case div3 nor div5 : print(x);      "
      "   };                                     "
      "   print('\n')                            "
      " until ((x += 1) > 100);                  ";

   exprtk::rtl::io::print<T> print;

   symbol_table_t symbol_table;
   symbol_table.add_function("print",print);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(fizzbuzz_program,expression);

   expression.value();
}

int main()
{
   fizzbuzz<double>();
   return 0;
}

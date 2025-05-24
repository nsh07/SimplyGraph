/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Recursive Fibonacci Example                         *
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


#include <string>

#include "exprtk.hpp"


template <typename T>
void recursive_fibonacci_example()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;

   symbol_table.add_package(io_package);

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("fibonacci")
      .var("n")
      .expression
      ( " (n < 2) ? n : fibonacci(n - 1) + fibonacci(n - 2); " ));


   const std::string fibonacci_program =
      " const var n := 35;                  "
      "                                     "
      " for (var i := 0; i <= n; i += 1)    "
      " {                                   "
      "    var fib_i := fibonacci(i);       "
      "    println('fib(',i,') = ', fib_i); "
      " }                                   ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(fibonacci_program,expression);

   exprtk::timer timer;
   timer.start();

   expression.value();

   timer.stop();

   printf("Total time: %8.4fsec\n",timer.time());
}

int main()
{
   recursive_fibonacci_example<double>();
   return 0;
}

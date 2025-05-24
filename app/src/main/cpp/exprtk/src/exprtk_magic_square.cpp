/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Magic Square                                        *
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
void magic_square()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   const T n = T(15);
   std::vector<T> square(static_cast<std::size_t>(n * n), T(0));

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;
   symbol_table.add_constant("n", n);
   symbol_table.add_vector  ("square", square);
   symbol_table.add_function("print_digit",
                             [](T v) ->T
                             { printf(" %03d ",static_cast<int>(v)); return 0; });
   symbol_table.add_package (io_package);

   compositor_t compositor(symbol_table);

   compositor.load_variables(true);
   compositor.load_vectors  (true);

   compositor.add(
      function_t("get_square")
      .vars("col", "row")
      .expression
      ( " square[row * n + col]; " ));

   compositor.add(
      function_t("set_square")
      .vars("col", "row", "value")
      .expression
      ( " square[row * n + col] := value; " ));

   compositor.add(
      function_t("display_magic_square")
      .expression
      (
         " for (var row := 0; row < n; row += 1)    "
         " {                                        "
         "    for (var col := 0; col < n; col += 1) "
         "    {                                     "
         "       print_digit(get_square(col,row));  "
         "    };                                    "
         "                                          "
         "    println('');                          "
         " }                                        "
      ));

   const std::string magic_square_program =
      " var num := 1;                           "
      " var i   := 0;                           "
      " var j   := floor(n / 2);                "
      "                                         "
      " while (num <= (n * n))                  "
      " {                                       "
      "    set_square(i, j, num);               "
      "    num += 1;                            "
      "                                         "
      "    var i_next := (i - 1 + n) % n;       "
      "    var j_next := (j + 1) % n;           "
      "                                         "
      "    if (get_square(i_next, j_next) != 0) "
      "    {                                    "
      "       i := (i + 1) % n;                 "
      "    }                                    "
      "    else                                 "
      "    {                                    "
      "       i := i_next;                      "
      "       j := j_next;                      "
      "    }                                    "
      " };                                      "
      "                                         "
      " println('sum: ', n * (n^2 + 1) / 2);    "
      "                                         "
      " display_magic_square();                 ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(magic_square_program,expression);

   expression.value();
}

int main()
{
   magic_square<double>();
   return 0;
}

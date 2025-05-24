/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Tower Of Hanoi Example                              *
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
void tower_of_hanoi_example()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   std::vector<T> disks = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;
   symbol_table.add_vector ("disks", disks);
   symbol_table.add_package(io_package);

   compositor_t compositor(symbol_table);

   compositor.load_variables(true);
   compositor.load_vectors  (true);

   compositor.add(
      function_t("move_disk")
      .vars("disk", "src_rod", "dest_rod")
      .expression
      (
         "println('Move disk', disk ,' from rod[', src_rod ,'] to rod[', dest_rod ,']');"
      ));

   compositor.add(
      function_t("tower_of_hanoi")
      .vars("n", "src", "dest", "aux")
      .expression
      (
         " if (n == 1)                            "
         " {                                      "
         "    move_disk(disks[0], src, dest);     "
         "    return [true];                      "
         " };                                     "
         "                                        "
         " tower_of_hanoi(n - 1, src, aux, dest); "
         "                                        "
         " move_disk(disks[n - 1], src, dest);    "
         "                                        "
         " tower_of_hanoi(n - 1, aux, dest, src); "
         "                                        "
      ));

   const std::string tower_of_hanoi_program =
      " tower_of_hanoi(disks[], 1, 2, 3); ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(tower_of_hanoi_program,expression);

   expression.value();
}

int main()
{
   tower_of_hanoi_example<double>();
   return 0;
}



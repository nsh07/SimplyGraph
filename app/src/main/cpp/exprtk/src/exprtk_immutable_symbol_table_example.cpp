/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Immutable Symbol Table Example                      *
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
#include <utility>
#include <algorithm>
#include <vector>

#include "exprtk.hpp"


template <typename T>
void immutable_symtab_example()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   T x = 1.1;
   T y = 2.2;
   T z = 3.3;
   T w = 4.4;

   symbol_table_t mutable_symbol_table;
   symbol_table_t immutable_symbol_table(symbol_table_t::symtab_mutability_type::e_immutable);

   mutable_symbol_table.add_variable("x", x);
   mutable_symbol_table.add_variable("y", y);

   immutable_symbol_table.add_variable("z", z);
   immutable_symbol_table.add_variable("w", w);

   expression_t expression;
   expression.register_symbol_table(immutable_symbol_table);
   expression.register_symbol_table(mutable_symbol_table  );

   parser_t parser;

   const std::vector<std::string> expressions =
   {
      "x := y + (z / w)",     // ok - will compile
      "y := y / x + (z / w)", // ok - will compile
      "z := y + x - w",       // Error - will not compile
      "z ==  (w := y / x)",   // Error - will not compile
   };

   for (const auto& expression_str : expressions)
   {

      if (!parser.compile(expression_str, expression))
      {
         for (std::size_t error_idx = 0; error_idx < parser.error_count(); ++error_idx)
         {
            const auto error = parser.get_error(error_idx);

            printf("Error: %02d Pos: %02d Type: [%14s] Message: %s\tExpression: %s\n",
                   static_cast<unsigned int>(error_idx),
                   static_cast<unsigned int>(error.token.position),
                   exprtk::parser_error::to_str(error.mode).c_str(),
                   error.diagnostic.c_str(),
                   expression_str.c_str());
         }

         continue;
      }

      // Modify all the variables from both the immutable
      // and mutable symbol tables

      x += 1.1;
      y += 2.2;
      z += 3.3;
      w += 4.4;

      expression.value();
   }

   return;
}

int main()
{
   immutable_symtab_example<double>();
   return 0;
}



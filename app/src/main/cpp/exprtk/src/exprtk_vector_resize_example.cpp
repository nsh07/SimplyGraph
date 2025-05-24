/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Vector Resize Example                                      *
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
#include <vector>

#include "exprtk.hpp"


template <typename T>
void vector_resize_example()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   std::vector<T> v0 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
   std::vector<T> v1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

   exprtk::vector_view<T> vv = exprtk::make_vector_view(v0, v0.size());

   T n = T(0);

   symbol_table_t symbol_table;
   symbol_table.add_variable("n", n);
   symbol_table.add_vector  ("v", vv);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   const std::string vector_resize_expression =
      " sum(v) == ((v[]^2 + v[]) / 2) and sum(2v) == (n^2 + n)";

   if (!parser.compile(vector_resize_expression, expression))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             vector_resize_expression.c_str());
      return;
   }

   for (std::size_t i = 1; i <= vv.base_size(); ++i)
   {
      vv.set_size(i);
      vv.rebase(i % 2 ? v0.data() : v1.data());

      n = T(i);

      const T result = expression.value();

      if (result != T(1))
      {
         printf("Error: vector size: %d\n", static_cast<unsigned int>(i));
      }
   }
}

int main()
{
   vector_resize_example<double>();
   return 0;
}


/*
   Build:
   c++ -pedantic-errors -Wall -Wextra -Werror -O3 -DNDEBUG -o vector_resize_example01 vector_resize_example01.cpp -L/usr/lib -lstdc++ -lm
*/

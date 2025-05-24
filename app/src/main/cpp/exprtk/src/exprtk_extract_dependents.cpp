/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Extract Expression Dependents                       *
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
void extract_expression_dependents()
{
   const std::string expression = "a * sin(3x) + foo(abs(y - z) / w)";

   exprtk::symbol_table<T> symbol_table;

   /*
      Note: Add any functions that are not part of the standard set
            of ExprTk functions
   */
   symbol_table.add_function("foo",[](T /*t*/)->T { return T(0); });

   std::deque<std::string> variable_list;
   std::deque<std::string> function_list;

   if (!exprtk::collect_variables(expression, symbol_table, variable_list))
   {
      printf("Error: Failed to collect variables for expression: %s\n",expression.c_str());
      variable_list.clear();
   }

   if (!exprtk::collect_functions(expression, symbol_table, function_list))
   {
      printf("Error: Failed to collect functions for expression: %s\n",expression.c_str());
      function_list.clear();
   }

   for (const auto& var : variable_list)
   {
      printf("variable: %s\n",var.c_str());
   }

   for (const auto& func : function_list)
   {
      printf("function: %s\n",func.c_str());
   }
}

int main()
{
   extract_expression_dependents<double>();
   return 0;
}


/*

   Build:
   c++ -pedantic-errors -Wall -Wextra -Werror -o exprtk_extract_dependents exprtk_extract_dependents.cpp -L/usr/lib -lstdc++ -lm

*/

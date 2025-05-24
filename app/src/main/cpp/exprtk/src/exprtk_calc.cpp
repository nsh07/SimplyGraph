/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Simple Calculator Example                           *
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
#include <iostream>
#include <string>

#include "exprtk.hpp"


int main()
{
   typedef exprtk::symbol_table<double> symbol_table_t;
   typedef exprtk::expression<double>   expression_t;
   typedef exprtk::parser<double>       parser_t;
   typedef exprtk::parser_error::type   err_t;

   for ( ; ; )
   {
      double x = 1.0;
      double y = 2.0;
      double z = 3.0;
      double w = 4.0;
      double u = 5.0;
      double v = 6.0;

      symbol_table_t symbol_table;
      symbol_table.add_variable("x",x);
      symbol_table.add_variable("y",y);
      symbol_table.add_variable("z",z);
      symbol_table.add_variable("w",w);
      symbol_table.add_variable("u",u);
      symbol_table.add_variable("v",v);
      symbol_table.add_constants();

      expression_t expression;
      expression.register_symbol_table(symbol_table);

      std::string expression_str;

      printf(">> ");
      std::getline(std::cin,expression_str);

      if (expression_str.empty())
         continue;
      else if ("exit" == expression_str)
         break;
      else if ("quit" == expression_str)
         break;

      parser_t parser;

      if (!parser.compile(expression_str,expression))
      {
         printf("Error: %s\tExpression: %s\n",
                parser.error().c_str(),
                expression_str.c_str());

         for (std::size_t i = 0; i < parser.error_count(); ++i)
         {
            err_t error = parser.get_error(i);
            printf("Error: %02d Pos: %02d Type: [%14s] Message: %s\tExpression: %s\n",
                   static_cast<unsigned int>(i),
                   static_cast<unsigned int>(error.token.position),
                   exprtk::parser_error::to_str(error.mode).c_str(),
                   error.diagnostic.c_str(),
                   expression_str.c_str());
         }

         continue;
      }

      double result = expression.value();

      printf("result: %20.10f\n",result);
   }

   return 0;
}

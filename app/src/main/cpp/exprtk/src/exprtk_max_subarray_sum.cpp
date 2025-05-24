/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Maximum Subarray Problem                            *
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
void max_subarray_sum()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string max_subarray_sum_program =
      " var zero       := 0;                "
      " var curr_sum   := 0;                "
      " var curr_start := 0;                "
      "                                     "
      " for (var i := 0; i < vec[]; i += 1) "
      " {                                   "
      "    curr_sum += vec[i];              "
      "                                     "
      "    if (curr_sum < zero)             "
      "    {                                "
      "       curr_sum   := 0;              "
      "       curr_start := i + 1;          "
      "    }                                "
      "    else if (curr_sum > max_sum)     "
      "    {                                "
      "       max_sum   := curr_sum;        "
      "       max_start := curr_start;      "
      "       max_end   := i;               "
      "    }                                "
      " }                                   ";

   T max_sum   = T(0);
   T max_start = T(0);
   T max_end   = T(0);

   T vec[] = { T(-1), T(-2), T(3), T(5), T(6), T(-2), T(-1), T(4), T(-4), T(2), T(-1) };

   symbol_table_t symbol_table;

   symbol_table.add_variable("max_sum"  , max_sum  );
   symbol_table.add_variable("max_start", max_start);
   symbol_table.add_variable("max_end"  , max_end  );
   symbol_table.add_vector  ("vec"      , vec      );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(max_subarray_sum_program,expression);

   expression.value();

   if (max_start <= max_end)
   {
      printf("Max sum: %10.3f \n", max_sum);
      printf("Start index: %4d\n", static_cast<int>(max_start));
      printf("End index:   %4d\n", static_cast<int>(max_end  ));

      for (int i = static_cast<int>(max_start); i <= static_cast<int>(max_end); ++i)
      {
         printf("%5.3f ",vec[i]);
      }

      printf("\n");
   }
   else
      printf("No maximum sum found.\n");
}

int main()
{
   max_subarray_sum<double>();
   return 0;
}

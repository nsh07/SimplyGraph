/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Compilation Timeout Check                           *
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


#include <chrono>
#include <cstdio>
#include <string>

#include "exprtk.hpp"


struct compilation_timeout_check final : public exprtk::compilation_check
{
   static constexpr std::size_t max_iters_per_check = 10000;

   bool continue_compilation(compilation_context& context) override
   {
      if (++iterations_ >= max_iters_per_check)
      {
         if (std::chrono::steady_clock::now() >= timeout_tp_)
         {
            context.error_message = "Compilation has timed-out";
            return false;
         }

         iterations_ = 0;
      }

      return true;
   }

   using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;

   void set_timeout_time(const time_point_t& timeout_tp)
   {
      timeout_tp_ = timeout_tp;
   }

   std::size_t iterations_ = max_iters_per_check;
   time_point_t timeout_tp_;
};

template <typename T>
void compilation_timeout_check_example()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string base_expression_string =
      "x := "
      "((1 /1) * sin( 2 * pi * f * t) + (1 /3) * sin( 6 * pi * f * t)+"
      " (1 /5) * sin(10 * pi * f * t) + (1 /7) * sin(14 * pi * f * t)+"
      " (1 /9) * sin(18 * pi * f * t) + (1/11) * sin(22 * pi * f * t)+"
      " (1/13) * sin(26 * pi * f * t) + (1/15) * sin(30 * pi * f * t)+"
      " (1/17) * sin(34 * pi * f * t) + (1/19) * sin(38 * pi * f * t)+"
      " (1/21) * sin(42 * pi * f * t) + (1/23) * sin(46 * pi * f * t)+"
      " (1/25) * sin(50 * pi * f * t) + (1/27) * sin(54 * pi * f * t));";

   std::string large_expression_string = "var x := 0;";

   for (std::size_t i = 0; i < 60000; ++i)
   {
      large_expression_string += base_expression_string;
   }

   static const T pi = T(3.141592653589793238462643383279502);

   const T f = pi / T(10);
   const T a = T(10);
         T t = T(0);

   symbol_table_t symbol_table;
   symbol_table.add_variable("t",t);
   symbol_table.add_constant("f",f);
   symbol_table.add_constant("a",a);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   compilation_timeout_check compilation_timeout_chck;

   parser_t parser;
   parser.register_compilation_timeout_check(compilation_timeout_chck);

   const auto max_duration = std::chrono::seconds(8);
   const auto timeout_tp   = std::chrono::steady_clock::now() + max_duration;
   compilation_timeout_chck.set_timeout_time(timeout_tp);

   if (!parser.compile(large_expression_string, expression))
   {
      printf("Error: %s\t\n", parser.error().c_str());
      return;
   }
}

int main()
{
   compilation_timeout_check_example<double>();
   return 0;
}

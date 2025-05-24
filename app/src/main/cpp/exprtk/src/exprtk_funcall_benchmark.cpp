/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Function Call Benchmark (Custom vs Composite vs Native)    *
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
struct func0 : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   func0()
   : exprtk::ifunction<T>(2)
   {}

   inline T operator()(const T& v1, const T& v2)
   {
      return T(1) + std::cos(v1 * v2) / T(3);
   }
};

template <typename T>
inline T func1(T v1, T v2)
{
   return T(1) + std::cos(v1 * v2) / T(3);
}

template <typename T>
void function_call_benchmark01()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   T v1 = T(1);
   T v2 = T(2);

   func0<T> f0;

   symbol_table_t symbol_table;

   symbol_table.add_constants();

   symbol_table.add_variable("v1"   , v1           );
   symbol_table.add_variable("v2"   , v2           );
   symbol_table.add_function("func0", f0           );
   symbol_table.add_function("func1", func1<double>);

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("func2")
      .var("v1").var("v2")
      .expression(" 1 + cos(v1 * v2) / 3;"));

   const std::string program0 = "func0(v1,v2);";
   const std::string program1 = "func1(v1,v2);";
   const std::string program2 = "func2(v1,v2);";

   expression_t expression0(symbol_table);
   expression_t expression1(symbol_table);
   expression_t expression2(symbol_table);

   parser_t parser;

   if (!parser.compile(program0,expression0))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             program0.c_str());

      return;
   }

   if (!parser.compile(program1,expression1))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             program1.c_str());

      return;
   }

   if (!parser.compile(program2,expression2))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             program1.c_str());

      return;
   }

   const std::size_t rounds = 100000000;

   {
      T total = T(0);

      exprtk::timer timer;
      timer.start();

      for (std::size_t r = 0; r < rounds; ++r)
      {
         total += expression0.value();
         std::swap(v1,v2);
      }

      timer.stop();

      printf("[custom function]  Total time: %8.3fsec\tRate: %15.3f\tTotal:%20.5f\n",
             timer.time(),
             rounds / timer.time(),
             total);
   }

   {
      T total = T(0);

      exprtk::timer timer;
      timer.start();

      for (std::size_t r = 0; r < rounds; ++r)
      {
         total += expression1.value();
         std::swap(v1,v2);
      }

      timer.stop();

      printf("[free function  ]  Total time: %8.3fsec\tRate: %15.3f\tTotal:%20.5f\n",
             timer.time(),
             rounds / timer.time(),
             total);
   }

   {
      T total = T(0);

      exprtk::timer timer;
      timer.start();

      for (std::size_t r = 0; r < rounds; ++r)
      {
         total += expression2.value();
         std::swap(v1,v2);
      }

      timer.stop();

      printf("[compositor     ]  Total time: %8.3fsec\tRate: %15.3f\tTotal:%20.5f\n",
             timer.time(),
             rounds / timer.time(),
             total);
   }

   {
      T total = T(0);

      exprtk::timer timer;
      timer.start();

      for (std::size_t r = 0; r < rounds; ++r)
      {
         total += func1<T>(v1,v2);
         std::swap(v1,v2);
      }

      timer.stop();

      printf("[native         ]  Total time: %8.3fsec\tRate: %15.3f\tTotal:%20.5f\n",
             timer.time(),
             rounds / timer.time(),
             total);
   }
}

int main()
{
   function_call_benchmark01<double>();
   return 0;
}

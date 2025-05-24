/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Multi-Threaded Vector Processing Benchmark          *
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
#include <fstream>
#include <limits>
#include <numeric>
#include <string>
#include <vector>
#include <thread>

#include "exprtk.hpp"


struct test_expression
{
   std::size_t cost;
   std::string expression;

   test_expression(const std::size_t c, const std::string& ex)
   : cost(c)
   , expression(ex)
   {}
};

const test_expression global_expression_list[] =
   {
      test_expression( 1, "2 * v0"                               ),
      test_expression( 1, "v0 * 2"                               ),
      test_expression( 2, "2v0 + 3"                              ),
      test_expression( 2, "3 + 2v0"                              ),
      test_expression( 5, "(2v0 + 3) * (2v0 + 3)"                ),
      test_expression( 5, "(3 + 2v0) * (3 + 2v0)"                ),
      test_expression( 1, "v0 + v1"                              ),
      test_expression( 3, "(v0 + v1) * (v0 - v1)"                ),
      test_expression( 3, "2v0 + 3v1"                            ),
      test_expression( 3, "2v0 - v1 / 3"                         ),
      test_expression( 2, "v0 * v1 / v2"                         ),
      test_expression( 3, "2(v0 * v1 / v2)"                      ),
      test_expression( 4, "2(v0 / 3 + v1 / 4)"                   ),
      test_expression( 3, "(2v0 - v1 / v2)"                      ),
      test_expression( 4, "3(2v0 - v1 / v2)"                     ),
      test_expression( 5, "7(5v0 * 3v1 / 2v2)"                   ),
      test_expression( 5, "abs(v0 - v1) * abs(v1 - v0)"          ),
      test_expression( 7, "abs(2v0 - v1) * abs(v1 - 2v0)"        ),
      test_expression( 9, "abs(2v0 - 3v1) * abs(3v1 - 5v0)"      ),
      test_expression( 2, "sum(2 * v0)"                          ),
      test_expression( 2, "sum(v0 * 2)"                          ),
      test_expression( 2, "sum(v0 * v1)"                         ),
      test_expression( 3, "sum(2v0 + 3)"                         ),
      test_expression( 3, "sum(3 + 2v0)"                         ),
      test_expression( 6, "sum((2v0 + 3) * (2v0 + 3))"           ),
      test_expression( 6, "sum((3 + 2v0) * (3 + 2v0))"           ),
      test_expression( 2, "sum(v0 + v1)"                         ),
      test_expression( 4, "sum((v0 + v1) * (v0 - v1))"           ),
      test_expression( 4, "sum(2v0 + 3v1)"                       ),
      test_expression( 4, "sum((2v0 - v1 / v2))"                 ),
      test_expression( 5, "sum(3(2v0 - v1 / v2))"                ),
      test_expression( 4, "sum(abs(v0 * v1) / v2)"               ),
      test_expression( 5, "sum(v0 + v1) + avg(v0 - v1)"          ),
      test_expression( 8, "7(sum(abs(5v0 * 3v1) / 2v2))"         ),
      test_expression( 9, "sum(2v0 + 3v1) + sum(4 / v0 - 5 / v1)"),
      test_expression( 6, "sum(abs(v0 - v1) * abs(v1 - v0))"     ),
      test_expression( 8, "sum(abs(2v0 - v1) * abs(v1 - 2v0))"   ),
      test_expression(10, "sum(abs(2v0 - 3v1) * abs(3v1 - 5v0))" ),
      test_expression( 2, "axpbz(2,v0,3,v1)"                     ),
      test_expression( 2, "axpy(2,v0,v1)"                        ),
    //test_expression( 4, "sum(v0^2.2 + v1^3.3)"                 ),
    //test_expression( 4, "exp(-1 / (v0^2))"                     ),
    //test_expression( 5, "exp(-1 / (v0^2)) / v1"                )
   };

const std::size_t global_expression_list_size = sizeof(global_expression_list) / sizeof(test_expression);

const std::size_t rounds = 2000;

typedef double numeric_type;

template <typename T>
void run_expression_benchmark(const std::size_t& vec_size,
                              const std::string& expression_string,
                              const std::size_t& cost)
{
   typedef exprtk::symbol_table<numeric_type> symbol_table_t;
   typedef exprtk::expression<numeric_type>   expression_t;
   typedef exprtk::parser<numeric_type>       parser_t;

   std::vector<T> v0(vec_size, T(3.1234567890));
   std::vector<T> v1(vec_size, T(5.1234567890));
   std::vector<T> v2(vec_size, T(7.1234567890));

   exprtk::rtl::vecops::package<T> vecops_package;

   symbol_table_t symbol_table;
   symbol_table.add_vector("v0",v0);
   symbol_table.add_vector("v1",v1);
   symbol_table.add_vector("v2",v2);
   symbol_table.add_package(vecops_package);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   if (!parser.compile(expression_string,expression))
   {
      printf("[load_expression] - Parser Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             expression_string.c_str());

      return;
   }

   exprtk::timer timer;

   T total = T(0);

   timer.start();

   for (std::size_t r = 0; r < rounds; ++r)
   {
      total += expression.value();
   }

   timer.stop();

   if (T(0) != total)
      printf("Total Time:%10.7f Rate:%11.3fevals/sec Perf: %5.3fGFLOPS Expression: %s\n",
             timer.time(),
             rounds / timer.time(),
             (rounds * vec_size * cost) / (1e+9 * timer.time()),
             expression_string.c_str());
   else
      printf("run_expression_benchmark() - Error running benchmark for expression: %s\n",
             expression_string.c_str());
}

template <typename T>
void run_threaded_benchmark(const std::size_t& vec_size, const std::size_t& thread_count)
{
   std::vector<std::thread> thread_list;

   exprtk::timer timer;

   timer.start();

   for (std::size_t i = 0; i < thread_count; ++i)
   {
      thread_list.emplace_back(
         std::thread([vec_size]()
                     {
                        for (std::size_t e = 0; e < global_expression_list_size; ++e)
                        {
                           run_expression_benchmark<T>(
                              vec_size,
                              global_expression_list[e].expression,
                              global_expression_list[e].cost);
                        }
                     }));
   }

   for (auto& t : thread_list)
   {
      t.join();
   }

   timer.stop();

   unsigned long long total_flops = 0;

   for (std::size_t i = 0; i < global_expression_list_size; ++i)
   {
      total_flops += global_expression_list[i].cost;
   }

   total_flops = (total_flops * rounds * vec_size * thread_count);

   printf("Total Time:%10.7f FLOP: %llu Perf: %7.4fGFLOPS\n",
          timer.time(),
          total_flops,
          total_flops / (1e9 * timer.time()));
}

int main(int argc, char* argv[])
{
   const std::size_t vec_size     = ((argc >= 2) ? atoi(argv[1]) : 100000);
   const std::size_t thread_count = ((argc == 3) ? atoi(argv[2]) : 1     );

   if (thread_count > 36)
      return -1;

   run_threaded_benchmark<double>(vec_size,thread_count);

   return 0;
}

/*
Build command:

c++ -pedantic-errors -Wall -Wextra -Werror -Wno-long-long \
-std=c++11                                                \
-O3 -DNDEBUG -mtune=native -march=native -ftree-vectorize \
-o exprtk_vector_benchmark_multithreaded                  \
exprtk_vector_benchmark_multithreaded.cpp                 \
-L/usr/lib -lstdc++ -lm -lpthread                         \

*/

/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Vector Processing Benchmark                         *
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
#include <string>
#include <vector>

#include "exprtk.hpp"


struct test_expression
{
   std::size_t cost;
   std::string e;

   test_expression(const std::size_t c, const std::string& ex)
   : cost(c)
   , e(ex)
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
T run_expression_benchmark(const std::size_t  vec_size,
                           const std::string& expr_string,
                           const std::size_t& cost)
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   std::vector<T> v0(vec_size, T(2.1234567890));
   std::vector<T> v1(vec_size, T(3.1234567890));
   std::vector<T> v2(vec_size, T(5.1234567890));

   exprtk::rtl::vecops::package<T> vecops_package;

   symbol_table_t symbol_table;
   symbol_table.add_vector("v0",v0);
   symbol_table.add_vector("v1",v1);
   symbol_table.add_vector("v2",v2);
   symbol_table.add_package(vecops_package);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   if (!parser.compile(expr_string,expression))
   {
      printf("[load_expression] - Parser Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             expr_string.c_str());

      return std::numeric_limits<T>::quiet_NaN();
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
             expr_string.c_str());
   else
      printf("run_expression_benchmark() - Error running benchmark for expression: %s\n",
             expr_string.c_str());

   return timer.time();
}

template <typename T>
void run_benchmark(const std::size_t& vec_size)
{
   T total_time = 0.0;

   for (std::size_t i = 0; i < global_expression_list_size; ++i)
   {
      total_time +=
      run_expression_benchmark<T>(vec_size,
                                  global_expression_list[i].e,
                                  global_expression_list[i].cost);
   }

   unsigned long long total_flops = 0;

   for (std::size_t i = 0; i < global_expression_list_size; ++i)
   {
      total_flops += global_expression_list[i].cost;
   }

   total_flops *= (rounds * vec_size);

   printf("Total Time:%10.7f FLOP: %llu Perf: %7.4fGFLOPS\n",
          total_time,
          total_flops,
          total_flops / (1e9 * total_time));
}

template <typename T>
void run_file_benchmark(const std::size_t& vec_size, const std::string& file_name)
{
   std::ifstream stream(file_name.c_str());

   if (!stream)
   {
      printf("run_file_benchmark() - Failed to open file: %s\n",
             file_name.c_str());
      return;
   }

   std::string line;
   std::vector<std::string> expr_list;

   while (std::getline(stream, line))
   {
      if (line.empty())
         continue;
      else if ('#' == line[0])
         continue;

      expr_list.push_back(line);
   }

   T total_time = T(0);

   for (std::size_t i = 0; i < expr_list.size(); ++i)
   {
      total_time += run_expression_benchmark<T>(vec_size,expr_list[i],0);
   }

   printf("Total Time:%10.7f\n", total_time);
}

int main(int argc, char* argv[])
{
   const std::size_t vec_size  = ((argc >= 2) ? atoi(argv[1]) : 100000);
   const std::string file_name = ((argc >  3) ? argv[2] : ""          );

   switch (argc)
   {
      case 2 : run_benchmark     <numeric_type>(vec_size          ); break;
      case 3 : run_file_benchmark<numeric_type>(vec_size,file_name); break;
   }

   return 0;
}

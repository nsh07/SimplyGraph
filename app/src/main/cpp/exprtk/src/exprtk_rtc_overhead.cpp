/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Vector RTC Overhead Analysis                        *
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


struct vector_access_rtc final : public exprtk::vector_access_runtime_check
{
   bool handle_runtime_violation(violation_context&) override
   {
      throw std::runtime_error("Runtime vector access violation.");
      return false;
   }
};

struct assert_handler final : public exprtk::assert_check
{
   void handle_assert(const assert_context& /*context*/) override
   {
      throw std::runtime_error("assert: vector access violation.");
   }
};

template <typename T>
void vector_rtc_overhead_analysis_01()
{
   typedef exprtk::expression<T> expression_t;
   typedef exprtk::parser<T>     parser_t;

   const std::string sieve_of_eratosthenes_program =
      " var sieve[10^8] := [false];                   "
      " var m := trunc(sqrt(sieve[]));                "
      "                                               "
      " sieve[0] := true;                             "
      " sieve[1] := true;                             "
      "                                               "
      " for (var i := 0; i <= m; i += 1)              "
      " {                                             "
      "    assert(i < sieve[]);                       "
      "    if (false == sieve[i])                     "
      "    {                                          "
      "       for (var j := i^2; j < sieve[]; j += i) "
      "       {                                       "
      "          assert(j < sieve[]);                 "
      "          sieve[j] := true;                    "
      "       }                                       "
      "    }                                          "
      " };                                            "
      "                                               "
      " var prime_count := sieve[] - sum(sieve);      "
      "                                               "
      " prime_count == 5761455;                       ";

   T nortc_result  = T(0);
   T rtc_result    = T(0);
   T assert_result = T(0);

   T nortc_time_sec  = T(0);
   T rtc_time_sec    = T(0);
   T assert_time_sec = T(0);

   {
      expression_t expression;

      parser_t parser;
      parser.compile(sieve_of_eratosthenes_program,expression);

      exprtk::timer timer;
      timer.start();

      nortc_result = expression.value();

      timer.stop();
      nortc_time_sec = timer.time();
   }

   {
      vector_access_rtc vec_rtc;

      expression_t expression;

      parser_t parser;
      parser.register_vector_access_runtime_check(vec_rtc);
      parser.compile(sieve_of_eratosthenes_program,expression);

      exprtk::timer timer;
      timer.start();

      rtc_result = expression.value();

      timer.stop();
      rtc_time_sec = timer.time();
   }

   {
      assert_handler asrt_handler;
      expression_t expression;

      parser_t parser;
      parser.register_assert_check(asrt_handler);
      parser.compile(sieve_of_eratosthenes_program,expression);

      exprtk::timer timer;
      timer.start();

      assert_result = expression.value();

      timer.stop();
      assert_time_sec = timer.time();
   }

   if (
        (nortc_result == T(1)         ) &&
        (nortc_result == rtc_result   ) &&
        (nortc_result == assert_result)
      )
   {
      printf("Prime Sieve - NORTC time: %5.3fsec\tRTC time: %5.3fsec\tassert time: %5.3fsec\trtc_overhead: %5.3f%%\tassert_overhead: %5.3f%%\n",
             nortc_time_sec,
             rtc_time_sec,
             assert_time_sec,
             std::max(0.0, 100.0 * ((rtc_time_sec    - nortc_time_sec) / nortc_time_sec)),
             std::max(0.0, 100.0 * ((assert_time_sec - nortc_time_sec) / nortc_time_sec)));
   }
   else
      printf("ERROR - Results of NORTC and RTC runs do not match!\n");
}

template <typename T>
void vector_rtc_overhead_analysis_02()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string bubble_sort_program =
      " var upper_bound := v[];                      "
      "                                              "
      " repeat                                       "
      "    var new_upper_bound := 0;                 "
      "                                              "
      "    for (var i := 1; i < upper_bound; i += 1) "
      "    {                                         "
      "       assert(i < v[]);                       "
      "       if (v[i - 1] > v[i])                   "
      "       {                                      "
      "          v[i - 1] <=> v[i];                  "
      "          new_upper_bound := i;               "
      "       };                                     "
      "    };                                        "
      "                                              "
      "    upper_bound := new_upper_bound;           "
      "                                              "
      " until (upper_bound <= 1);                    "
      "                                              "
      " var result := true;                          "
      "                                              "
      " for (var i := 1; i < v[]; i += 1)            "
      " {                                            "
      "    assert(i < v[]);                          "
      "    if (v[i - 1] > v[i])                      "
      "    {                                         "
      "       result := false;                       "
      "    };                                        "
      " };                                           "
      "                                              "
      " result;                                      ";

   T nortc_result  = T(0);
   T rtc_result    = T(0);
   T assert_result = T(0);

   T nortc_time_sec  = T(0);
   T rtc_time_sec    = T(0);
   T assert_time_sec = T(0);

   const std::size_t num_values = 10000;
   std::vector<T> values(num_values, 0.0);

   for (std::size_t i = 0; i < values.size(); ++i)
   {
      values[i] = static_cast<double>(values.size() - i);
   }

   {
      std::vector<T> v = values;

      symbol_table_t symbol_table;
      symbol_table.add_vector("v", v);

      expression_t expression;
      expression.register_symbol_table(symbol_table);

      parser_t parser;
      parser.compile(bubble_sort_program, expression);

      exprtk::timer timer;
      timer.start();

      nortc_result = expression.value();

      timer.stop();
      nortc_time_sec = timer.time();
   }

   {
      std::vector<T> v = values;

      symbol_table_t symbol_table;
      symbol_table.add_vector("v", v);

      vector_access_rtc vec_rtc;

      expression_t expression;
      expression.register_symbol_table(symbol_table);

      parser_t parser;
      parser.register_vector_access_runtime_check(vec_rtc);
      parser.compile(bubble_sort_program, expression);

      exprtk::timer timer;
      timer.start();

      rtc_result = expression.value();

      timer.stop();
      rtc_time_sec = timer.time();
   }

   {
      std::vector<T> v = values;

      symbol_table_t symbol_table;
      symbol_table.add_vector("v", v);

      expression_t expression;
      expression.register_symbol_table(symbol_table);

      assert_handler asrt_handler;

      parser_t parser;
      parser.register_assert_check(asrt_handler);
      parser.compile(bubble_sort_program, expression);

      exprtk::timer timer;
      timer.start();

      assert_result = expression.value();

      timer.stop();
      assert_time_sec = timer.time();
   }

   if (
        (nortc_result == T(1)         ) &&
        (nortc_result == rtc_result   ) &&
        (nortc_result == assert_result)
      )
   {
      printf("Bubble Sort - NORTC time: %5.3fsec\tRTC time: %5.3fsec\tassert time: %5.3fsec\trtc_overhead: %5.3f%%\tassert_overhead: %5.3f%%\n",
             nortc_time_sec,
             rtc_time_sec,
             assert_time_sec,
             std::max(0.0, 100.0 * ((rtc_time_sec    - nortc_time_sec) / nortc_time_sec)),
             std::max(0.0, 100.0 * ((assert_time_sec - nortc_time_sec) / nortc_time_sec)));
   }
   else
      printf("ERROR - Results of NORTC and RTC runs do not match!\n");
}

int main(int argc, char* argv[])
{
   const std::size_t rounds = ((argc == 2) ? atoi(argv[1]) : 5);

   for (std::size_t i = 0; i < rounds; ++i)
   {
      vector_rtc_overhead_analysis_01<double>();
   }

   printf("-------------------------\n");

   for (std::size_t i = 0; i < rounds; ++i)
   {
      vector_rtc_overhead_analysis_02<double>();
   }

   return 0;
}

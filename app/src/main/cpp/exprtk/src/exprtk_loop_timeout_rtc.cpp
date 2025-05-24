/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Loop Timeout RTC                                    *
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


struct timeout_rtc_handler final : public exprtk::loop_runtime_check
{
   timeout_rtc_handler()
   : exprtk::loop_runtime_check()
   {}

   class timeout_exception final : public std::runtime_error
   {
   public:

      timeout_exception(const std::string& what = "")
      : std::runtime_error(what)
      {}
   };

   using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;

   void set_timeout_time(const time_point_t& timeout_tp)
   {
      timeout_tp_ = timeout_tp;
   }

   bool check() override
   {
      static constexpr std::size_t max_iters_per_check = 100000;

      if (++iterations_ >= max_iters_per_check)
      {
         if (std::chrono::steady_clock::now() >= timeout_tp_)
         {
            return false;
         }

         iterations_ = 0;
      }

      return true;
   }

   void handle_runtime_violation(const violation_context&) override
   {
      throw timeout_exception("Loop run-time timeout violation.");
   }

   std::size_t iterations_ = 0;
   time_point_t timeout_tp_;
};

struct vector_access_rtc final : public exprtk::vector_access_runtime_check
{
   bool handle_runtime_violation(violation_context&) override
   {
      throw std::runtime_error("Runtime vector access violation.");
      return false;
   }
};

template <typename T>
void loop_timeout_rtc()
{
   typedef exprtk::expression<T>      expression_t;
   typedef exprtk::parser<T>          parser_t;
   typedef exprtk::loop_runtime_check loop_runtime_check_t;

   const std::string sieve_of_eratosthenes_program =
      " var sieve[10^8] := [false];                   "
      " var m := trunc(sqrt(sieve[]));                "
      "                                               "
      " sieve[0] := true;                             "
      " sieve[1] := true;                             "
      "                                               "
      " for (var i := 0; i <= m; i += 1)              "
      " {                                             "
      "    if (false == sieve[i])                     "
      "    {                                          "
      "       for (var j := i^2; j < sieve[]; j += i) "
      "       {                                       "
      "          sieve[j] := true;                    "
      "       }                                       "
      "    }                                          "
      " };                                            "
      "                                               "
      " var prime_count := sieve[] - sum(sieve);      "
      "                                               "
      " prime_count == 5761455;                       ";

   timeout_rtc_handler loop_runtime_check;
   loop_runtime_check.loop_set = loop_runtime_check_t::e_all_loops;
   loop_runtime_check.max_loop_iterations =
      static_cast<std::uint64_t>(10e8 * std::log10(std::log10(10e8))); // O(n) = nlog(log(n))

   vector_access_rtc vec_rtc;

   expression_t expression;

   parser_t parser;

   parser.register_loop_runtime_check(loop_runtime_check);
   parser.register_vector_access_runtime_check(vec_rtc);

   parser.compile(sieve_of_eratosthenes_program,expression);

   const auto max_duration = std::chrono::seconds(1);
   const auto timeout_tp   = std::chrono::steady_clock::now() + max_duration;
   loop_runtime_check.set_timeout_time(timeout_tp);

   try
   {
      expression.value();
   }
   catch (timeout_rtc_handler::timeout_exception& /*toe*/)
   {
      printf("Timeout Exception\n");
   }
   catch (std::runtime_error& rte)
   {
      printf("Exception: %s\n",rte.what());
   }

   parser.clear_loop_runtime_check();
}

int main()
{
   loop_timeout_rtc<double>();
   return 0;
}

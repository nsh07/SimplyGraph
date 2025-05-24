/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Monte-Carlo Based European Option Pricing Model     *
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


#include <algorithm>
#include <array>
#include <cstdio>
#include <random>
#include <string>

#include "exprtk.hpp"


template <typename T>
struct normal_distribution final : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   normal_distribution()
   : exprtk::ifunction<T>(2)
   {
      std::random_device device;
      std::array<unsigned int,std::mt19937::state_size> seed;
      std::generate_n(seed.data(), seed.size(), std::ref(device));
      std::seed_seq seq(std::begin(seed), std::end(seed));
      generator.seed(seq);
   }

   inline T operator()(const T& mean, const T& stddev) override
   {
      std::normal_distribution<T> distribution{mean, stddev};
      return distribution(generator);
   }

   std::mt19937 generator;
};

template <typename T>
void exprtk_montecarlo_option_pricing_model()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string exprtk_montecarlo_option_pricing_model_program =
      " var payoff_sum := 0;                                                 "
      " var sqrt_t     := sqrt(t);                                           "
      "                                                                      "
      " for (var i := 0; i < n; i += 1)                                      "
      " {                                                                    "
      "    var s_t := s * exp((r - v^2 / 2) * t + v * sqrt_t * normal(0,1)); "
      "    payoff_sum +=                                                     "
      "       switch                                                         "
      "       {                                                              "
      "          case callput_flag == 'call' : max(s_t - k, 0);              "
      "          case callput_flag == 'put'  : max(k - s_t, 0);              "
      "       };                                                             "
      " };                                                                   "
      "                                                                      "
      " exp(-r * t) * payoff_sum / n;                                        ";

   T s = T(100.00); // Spot / Stock / Underlying / Base price
   T k = T(110.00); // Strike price
   T v = T(  0.30); // Volatility
   T t = T(  2.22); // Years to maturity
   T r = T(  0.05); // Risk free rate
   T n = T( 2.0e7); // Number of simulations

   std::string callput_flag;

   normal_distribution<T> normal;

   symbol_table_t symbol_table(symbol_table_t::e_immutable);
   symbol_table.add_variable("s",s);
   symbol_table.add_variable("k",k);
   symbol_table.add_variable("t",t);
   symbol_table.add_variable("r",r);
   symbol_table.add_variable("v",v);
   symbol_table.add_constant("n",n);
   symbol_table.add_stringvar("callput_flag", callput_flag);
   symbol_table.add_function ("normal"      , normal      );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(exprtk_montecarlo_option_pricing_model_program, expression);

   callput_flag = "call";

   const T montecarlo_call_option_price = expression.value();

   callput_flag = "put";

   const T montecarlo_put_option_price = expression.value();

   printf("MCOptionPrice(call, %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %10.6f\n",
          s, k, t, r, v,
          montecarlo_call_option_price);

   printf("MCOptionPrice(put , %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %10.6f\n",
          s, k, t, r, v,
          montecarlo_put_option_price);

   const double put_call_parity_diff =
      (montecarlo_call_option_price - montecarlo_put_option_price) -
      (s - k * std::exp(-r * t));

   printf("Put-Call parity difference: %20.17f\n", put_call_parity_diff);
}

int main()
{
   exprtk_montecarlo_option_pricing_model<double>();
   return 0;
}

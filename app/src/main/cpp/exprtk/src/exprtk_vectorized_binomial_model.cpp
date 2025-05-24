/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Vectorized Binomial Option Pricing Model                   *
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
void vectorized_binomial_option_pricing_model()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string european_option_binomial_model_program =
      " var dt     := t / n;                                           "
      " var z      := exp(r * dt);                                     "
      " var z_inv  := 1 / z;                                           "
      " var u      := exp(v * sqrt(dt));                               "
      " var u_inv  := 1 / u;                                           "
      " var p_up   := (z - u_inv) / (u - u_inv);                       "
      " var p_down := 1 - p_up;                                        "
      "                                                                "
      " var option_price[n + 1] := [0];                                "
      "                                                                "
      " for (var i := 0; i <= n; i += 1)                               "
      " {                                                              "
      "    var base_price  := s * u^(n - 2i);                          "
      "    option_price[i] :=                                          "
      "       switch                                                   "
      "       {                                                        "
      "          case callput_flag == 'call' : max(base_price - k, 0); "
      "          case callput_flag == 'put'  : max(k - base_price, 0); "
      "       };                                                       "
      " };                                                             "
      "                                                                "
      " var p_u_zinv := z_inv * p_up;                                  "
      " var p_d_zinv := z_inv * p_down;                                "
      "                                                                "
      " for (var j := n - 1; j >= 0; j -= 1)                           "
      " {                                                              "
      "    /* y_i <- a * x_i + b * y_(i+1) i:[0,j] */                  "
      "    axpbsy(p_u_zinv,    option_price,                           "
      "           p_d_zinv, 1, option_price,                           "
      "           0, j);                                               "
      " };                                                             "
      "                                                                "
      " option_price[0];                                               ";


   T s = T( 100.00); // Spot / Stock / Underlying / Base price
   T k = T( 110.00); // Strike price
   T v = T(   0.30); // Volatility
   T t = T(   2.22); // Years to maturity
   T r = T(   0.05); // Risk free rate
   T n = T(2000.00); // Number of time steps

   std::string callput_flag;

   exprtk::rtl::vecops::package<T> vecops_package;

   symbol_table_t symbol_table;
   symbol_table.add_variable("s",s);
   symbol_table.add_variable("k",k);
   symbol_table.add_variable("t",t);
   symbol_table.add_variable("r",r);
   symbol_table.add_variable("v",v);
   symbol_table.add_constant("n",n);
   symbol_table.add_stringvar("callput_flag",callput_flag);
   symbol_table.add_package(vecops_package);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(european_option_binomial_model_program,expression);

   const std::size_t rounds = 2000;

   T binomial_call_option_price = T(0);
   T binomial_put_option_price  = T(0);

   {
      callput_flag = "call";

      exprtk::timer timer;
      timer.start();

      for (std::size_t i = 0; i < rounds; ++i)
      {
         binomial_call_option_price = expression.value();
      }

      timer.stop();

      printf("BinomialOptionPrice(Type: %4s, BasePx: %5.3f, Strike: %5.3f, Time: %5.3f, RFR: %5.3f, Vol: %5.3f, Steps: %4.1f) = %10.6f "
             "total time: %6.3fsec rate: %6.3fcalc/sec\n",
             callput_flag.c_str(),
             s, k, t, r, v, n,
             binomial_call_option_price,
             timer.time(),
             rounds / timer.time());
   }

   {
      callput_flag = "put";

      exprtk::timer timer;
      timer.start();

      for (std::size_t i = 0; i < rounds; ++i)
      {
         binomial_put_option_price = expression.value();
      }

      timer.stop();

      printf("BinomialOptionPrice(Type: %4s, BasePx: %5.3f, Strike: %5.3f, Time: %5.3f, RFR: %5.3f, Vol: %5.3f, Steps: %4.1f) = %10.6f "
             "total time: %6.3fsec rate: %6.3fcalc/sec\n",
             callput_flag.c_str(),
             s, k, t, r, v, n,
             binomial_put_option_price,
             timer.time(),
             rounds / timer.time());
   }

   const T put_call_parity_diff =
      (binomial_call_option_price - binomial_put_option_price) -
      (s - k * std::exp(-r * t));

   printf("Put-Call parity difference: %20.17f\n", put_call_parity_diff);
}

int main()
{
   vectorized_binomial_option_pricing_model<double>();
   return 0;
}

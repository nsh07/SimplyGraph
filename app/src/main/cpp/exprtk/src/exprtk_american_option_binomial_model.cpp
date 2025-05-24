/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * American Option Binomial Pricing Model                     *
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
void american_option_binomial_option_pricing_model()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string american_option_binomial_model_program =
      " var dt       := t / n;                                                               "
      " var z        := exp(r * dt);                                                         "
      " var u        := exp(v * sqrt(dt));                                                   "
      " var p_up     := (z * u - 1) / (u^2 - 1);                                             "
      " var p_down   := 1 - p_up;                                                            "
      " var discount := 1 / z;                                                               "
      "                                                                                      "
      " var option_price[n + 1] := [0];                                                      "
      "                                                                                      "
      " for (var i := 0; i <= n; i += 1)                                                     "
      " {                                                                                    "
      "    var base_price := s * u^(2 * i - n);                                              "
      "                                                                                      "
      "    option_price[i] :=                                                                "
      "       switch                                                                         "
      "       {                                                                              "
      "          case callput_flag == 'call' : max(base_price - k, 0);                       "
      "          case callput_flag == 'put'  : max(k - base_price, 0);                       "
      "       };                                                                             "
      " };                                                                                   "
      "                                                                                      "
      " for (var j := n - 1; j >= 0; j -= 1)                                                 "
      " {                                                                                    "
      "    for (var i := 0; i <= j; i += 1)                                                  "
      "    {                                                                                 "
      "       var base_price_at_node := s * u^(2 * i - j);                                   "
      "                                                                                      "
      "       var intrinsic_value :=                                                         "
      "          switch                                                                      "
      "          {                                                                           "
      "             case callput_flag == 'call' : base_price_at_node - k;                    "
      "             case callput_flag == 'put'  : k - base_price_at_node;                    "
      "          };                                                                          "
      "                                                                                      "
      "       var expected_value := discount *                                               "
      "                             (p_up * option_price[i + 1] + p_down * option_price[i]); "
      "                                                                                      "
      "       option_price[i] := max(expected_value, intrinsic_value);                       "
      "    }                                                                                 "
      " };                                                                                   "
      "                                                                                      "
      " option_price[0];                                                                     ";

   T s = T( 100.00); // Spot / Stock / Underlying / Base price
   T k = T( 110.00); // Strike price
   T v = T(   0.30); // Volatility
   T t = T(   2.22); // Years to maturity
   T r = T(   0.05); // Risk free rate
   T n = T(1000.00); // Number of time steps

   std::string callput_flag;

   symbol_table_t symbol_table(symbol_table_t::e_immutable);
   symbol_table.add_variable("s", s);
   symbol_table.add_variable("k", k);
   symbol_table.add_variable("t", t);
   symbol_table.add_variable("r", r);
   symbol_table.add_variable("v", v);
   symbol_table.add_constant("n", n);
   symbol_table.add_stringvar("callput_flag",callput_flag);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(american_option_binomial_model_program,expression);

   callput_flag = "call";

   const T binomial_call_option_price = expression.value();

   callput_flag = "put";

   const T binomial_put_option_price = expression.value();

   printf("American BinomialPrice(call, %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %22.18f\n",
          s, k, t, r, v,
          binomial_call_option_price);

   printf("American BinomialPrice(put , %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %22.18f\n",
          s, k, t, r, v,
          binomial_put_option_price);

   // American option put-call 'parity': s - k < call - put < s - k * e^(-rt)
   const T    callput_diff    = (binomial_call_option_price - binomial_put_option_price);
   const T    basestrike_diff = s - k;
   const T    basepv_diff     = s - k * std::exp(-r * t);
   const bool put_call_parity = (basestrike_diff < callput_diff) &&
                                (callput_diff    < basepv_diff ) ;

   const T call_price_r0 = binomial_put_option_price + basestrike_diff;
   const T call_price_r1 = binomial_put_option_price + basepv_diff;

   const T put_price_r0  = binomial_call_option_price - basepv_diff;
   const T put_price_r1  = binomial_call_option_price - basestrike_diff;

   printf("Put-Call parity: %s\n", put_call_parity ? "True" : "False");

   printf("Call price range: %7.4f < %7.4f < %7.4f\n",
          call_price_r0,
          binomial_call_option_price,
          call_price_r1);

   printf("Put price range:  %7.4f < %7.4f < %7.4f\n",
          put_price_r0,
          binomial_put_option_price,
          put_price_r1);
}

int main()
{
   american_option_binomial_option_pricing_model<double>();
   return 0;
}

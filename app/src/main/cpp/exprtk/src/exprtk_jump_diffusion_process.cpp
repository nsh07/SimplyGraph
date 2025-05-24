/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Merton Jump Diffusion Process Option Pricing Model         *
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
void european_option_merton_jump_diffusion_process()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   const std::string european_option_merton_jump_diffusion_process_program =
      " var lambda_t   := lambda * t;                                                  "
      " var v_sqr      := v^2;                                                         "
      " var sigmaJ_sqr := sigmaJ^2;                                                    "
      "                                                                                "
      " var option_price := 0;                                                         "
      " var factorial    := 1;                                                         "
      "                                                                                "
      " for (var i := 0; i < n; i += 1)                                                "
      " {                                                                              "
      "    var prob    := exp(-lambda_t) * lambda_t^i / factorial;                     "
      "    var r_i     := r - lambda * muJ + (i / t) * log(1 + muJ);                   "
      "    var sigma_i := sqrt(v_sqr + (i * sigmaJ_sqr) / t);                          "
      "                                                                                "
      "    option_price +=                                                             "
      "       switch                                                                   "
      "       {                                                                        "
      "          case callput_flag == 'call' : prob * bsm_call(s, k, r_i, t, sigma_i); "
      "          case callput_flag == 'put'  : prob * bsm_put (s, k, r_i, t, sigma_i); "
      "       };                                                                       "
      "                                                                                "
      "    factorial *= (i > 1) ? i : 1;                                               "
      " };                                                                             "
      "                                                                                "
      " option_price;                                                                  ";

   T s      = T(100.00); // Spot / Stock / Underlying / Base price
   T k      = T(110.00); // Strike price
   T v      = T(  0.30); // Volatility
   T t      = T(  2.22); // Years to maturity
   T r      = T(  0.05); // Risk free rate
   T lambda = T(0.0001); // Jump intensity (average jumps per year)
   T muJ    = T( -0.05); // Mean jump size (negative for downward jumps)
   T sigmaJ = T(  0.30); // Standard deviation of the jump size
   T n      = T( 50.00); // Number of terms in the Poisson sum

   std::string callput_flag;

   symbol_table_t symbol_table(symbol_table_t::e_immutable);
   symbol_table.add_variable ("s"     , s     );
   symbol_table.add_variable ("k"     , k     );
   symbol_table.add_variable ("v"     , v     );
   symbol_table.add_variable ("t"     , t     );
   symbol_table.add_variable ("r"     , r     );
   symbol_table.add_variable ("lambda", lambda);
   symbol_table.add_variable ("muJ"   , muJ   );
   symbol_table.add_variable ("sigmaJ", sigmaJ);
   symbol_table.add_variable ("n"     , n     );
   symbol_table.add_stringvar("callput_flag",callput_flag);
   symbol_table.add_pi();

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("bsm_call")
      .vars("s", "k", "r", "t", "v")
      .expression
      (
         " var d1 := (log(s / k) + (r + v^2 / 2) * t) / (v * sqrt(t)); "
         " var d2 := d1 - v * sqrt(t);                                 "
         " s * ncdf(d1) - k * exp(-r * t) * ncdf(d2);                  "
      ));

   compositor.add(
      function_t("bsm_put")
      .vars("s", "k", "r", "t", "v")
      .expression
      (
         " var d1 := (log(s / k) + (r + v^2 / 2) * t) / (v * sqrt(t)); "
         " var d2 := d1 - v * sqrt(t);                                 "
         " k * exp(-r * t) * ncdf(-d2) - s * ncdf(-d1);                "
      ));

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(european_option_merton_jump_diffusion_process_program, expression);

   callput_flag = "call";

   const T jdp_call_option_price = expression.value();

   callput_flag = "put";

   const T jdp_put_option_price = expression.value();

   printf("JDPPrice(%4s, %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %10.6f\n",
          "call",
          s, k, t, r, v,
          jdp_call_option_price);

   printf("JDPPrice(%4s, %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %10.6f\n",
          "put",
          s, k, t, r, v,
          jdp_put_option_price);

   const T put_call_parity_diff =
      (jdp_call_option_price - jdp_put_option_price) -
      (s - k * std::exp(-r * t));

   printf("Put-Call parity difference: %20.17f\n", put_call_parity_diff);
}

int main()
{
   european_option_merton_jump_diffusion_process<double>();
   return 0;
}

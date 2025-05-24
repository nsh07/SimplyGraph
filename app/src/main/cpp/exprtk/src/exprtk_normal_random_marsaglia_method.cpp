/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Normal Random Variables Via Marsaglia Method        *
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
struct uniform_random_01 final : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   uniform_random_01()
   : exprtk::ifunction<T>(0)
   {
      std::random_device device;
      std::array<unsigned int,std::mt19937::state_size> seed;
      std::generate_n(seed.data(), seed.size(), std::ref(device));
      std::seed_seq seq(std::begin(seed), std::end(seed));
      generator.seed(seq);
   }

   inline T operator()() override
   {
      return distribution(generator);
   }

   std::mt19937 generator;
   std::uniform_real_distribution<T> distribution{ T(0), T(1) };
};

template <typename T>
void normal_random_marsaglia_method()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   exprtk::rtl::io::println<T> println;
   uniform_random_01<T>        unirandom;

   symbol_table_t symbol_table;
   symbol_table.add_function("println", println  );
   symbol_table.add_function("random" , unirandom);
   symbol_table.add_constants();

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("normal_distribution")
      .vars("mean", "stddev")
      .expression
      (
         " var u := 0;                 "
         " var v := 0;                 "
         " var s := 0;                 "
         "                             "
         " repeat                      "
         "    u := random() * 2 - 1;   "
         "    v := random() * 2 - 1;   "
         "    s := u^2 + v^2;          "
         " until (s > 0 and s < 1);    "
         "                             "
         " s := sqrt(-2 * log(s) / s); "
         " mean + stddev * u * s;      "
      ));

   const std::string normal_random_marsaglia_method_program =
      " const var mean          := pi;                                                      "
      " const var stddev        := 3 * mean / 2;                                            "
      " const var num_samples   := 1e7;                                                     "
      " var values[num_samples] := [normal_distribution(mean, stddev)];                     "
      "                                                                                     "
      " var sample_mean   := avg(values);                                                   "
      " var sample_stddev := sqrt(sum([values - sample_mean]^2) / values[]);                "
      "                                                                                     "
      " println('sample mean:   ', sample_mean  , ' error: ', abs(sample_mean - mean));     "
      " println('sample stddev: ', sample_stddev, ' error: ', abs(sample_stddev - stddev)); "
      "                                                                                     "
      "                                                                                     ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(normal_random_marsaglia_method_program,expression);

   expression.value();
}

int main()
{
   normal_random_marsaglia_method<double>();
   return 0;
}

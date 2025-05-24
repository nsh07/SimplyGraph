/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Ornstein-Uhlenbeck Process                          *
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
void ornstein_uhlenbeck_process()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string ornstein_uhlenbeck_process_program =
      " const var mu        := 1;                                                        "
      " const var sigma     := 0.4;                                                      "
      " const var theta     := 2;                                                        "
      " const var tau       := 1 / theta;                                                "
      " const var T         := 10 * tau;                                                 "
      " const var dt        := 0.01 * tau;                                               "
      " const var num_steps := floor(T / dt);                                            "
      " const var stddev    := sqrt(sigma^2 / (2 * theta) * (1 - exp(-2 * theta * dt))); "
      " const var x_0       := 1.0;                                                      "
      "                                                                                  "
      " var x[num_steps] := [0];                                                         "
      "                                                                                  "
      " x[0] := x_0;                                                                     "
      "                                                                                  "
      " for (var i := 0; i < num_steps - 1; i += 1)                                      "
      " {                                                                                "
      "    var mean := x[i] * exp(-theta * dt) + mu * (1 - exp(-theta * dt));            "
      "                                                                                  "
      "    x[i + 1] := normal(mean,stddev);                                              "
      " };                                                                               "
      "                                                                                  "
      "                                                                                  "
      " for (var i := 0; i < x[]; i += 1)                                                "
      " {                                                                                "
      "    println(i * dt, '\t', x[i]);                                                  "
      " }                                                                                ";


   exprtk::rtl::io::println<T> println;
   normal_distribution<T>      normal;

   symbol_table_t symbol_table;
   symbol_table.add_function("println", println);
   symbol_table.add_function("normal" , normal );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(ornstein_uhlenbeck_process_program,expression);

   expression.value();
}

int main()
{
   ornstein_uhlenbeck_process<double>();
   return 0;
}

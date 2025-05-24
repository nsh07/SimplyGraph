/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Conway's Game Of Life                               *
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
#include <chrono>
#include <cstdio>
#include <random>
#include <string>
#include <thread>

#include "exprtk.hpp"


template <typename T>
struct rnd_01 final : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   rnd_01()
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
   std::uniform_real_distribution<T> distribution{T(0),T(1)};
};

template <typename T>
void game_of_life()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   const std::size_t width  = 60;
   const std::size_t height = 30;

   std::vector<T> world(width * height, T(0));
   rnd_01<T> rnd01;
   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;
   symbol_table.add_constant("width" , static_cast<T>(width ));
   symbol_table.add_constant("height", static_cast<T>(height));
   symbol_table.add_vector  ("world" , world);
   symbol_table.add_package (io_package);
   symbol_table.add_function("random", rnd01);
   symbol_table.add_function("sleep" ,
                             [](T time_ms) -> T
                             {
                               std::this_thread::sleep_for(
                               std::chrono::milliseconds(static_cast<std::size_t>(time_ms)));
                               return T(1);
                             });

   compositor_t compositor(symbol_table);

   compositor.load_variables(true);
   compositor.load_vectors  (true);


   compositor.add(
      function_t("point")
      .vars("x","y")
      .expression
      (
         " world[y * width + x]; "
      ));

   compositor.add(
      function_t("set_point")
      .vars("x","y","value")
      .expression
      (
         " world[y * width + x] := value; "
      ));

   compositor.add(
      function_t("render")
      .expression
      (
         " print('+');                                         "
         " for (var x := 0; x < width; x += 1) { print('-'); } "
         " println('+');                                       "
         "                                                     "
         " for (var y := 0; y < height; y += 1)                "
         " {                                                   "
         "    print('|');                                      "
         "    for (var x := 0; x < width; x += 1)              "
         "    {                                                "
         "       print( point(x,y) ? '*' : ' ' );              "
         "    };                                               "
         "    println('|');                                    "
         " };                                                  "
         "                                                     "
         " print('+');                                         "
         " for (var x := 0; x < width; x += 1) { print('-'); } "
         " println('+');                                       "
      ));

   compositor.add(
      function_t("evolve")
      .expression
      (
         " var next_world[world[]] := [0];                       "
         "                                                       "
         " for (var y := 0; y < height; y += 1)                  "
         " {                                                     "
         "    for (var x := 0; x < width; x += 1)                "
         "    {                                                  "
         "       var alive_count := point(x,y) ? -1 : 0;         "
         "                                                       "
         "       for (var y1 := y - 1; y1 <= y + 1; y1 += 1)     "
         "       {                                               "
         "          var curr_y := (y1 + height) % height;        "
         "                                                       "
         "          for (var x1 := x - 1; x1 <= x + 1; x1 += 1)  "
         "          {                                            "
         "             var curr_x := (x1 + width) % width;       "
         "                                                       "
         "             if (point(curr_x,curr_y))                 "
         "             {                                         "
         "                alive_count += 1;                      "
         "             }                                         "
         "          }                                            "
         "       };                                              "
         "                                                       "
         "       next_world[y * width + x] :=                    "
         "          switch                                       "
         "          {                                            "
         "             case alive_count == 2 and point(x,y) : 1; "
         "             case alive_count == 3 : 1;                "
         "             default               : 0;                "
         "          };                                           "
         "    }                                                  "
         " };                                                    "
         "                                                       "
         " world := next_world;                                  "
      ));

   const std::string game_of_life_driver =
      " /* Randomly setup the initial state of the world */ "
      " for (var x := 0; x < width; x += 1)                 "
      " {                                                   "
      "    for (var y := 0; y < height; y += 1)             "
      "    {                                                "
      "       set_point(x, y, (random() < 0.15) ? 1 : 0);   "
      "    }                                                "
      " };                                                  "
      "                                                     "
      " var num_generations := 200;                         "
      "                                                     "
      " for (var i := 0; i < num_generations; i += 1)       "
      " {                                                   "
      "    println('Generation: ', i);                      "
      "    render();                                        "
      "    evolve();                                        "
      "    sleep(200);                                      "
      " }                                                   ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(game_of_life_driver,expression);

   expression.value();
}

int main()
{
   game_of_life<double>();
   return 0;
}

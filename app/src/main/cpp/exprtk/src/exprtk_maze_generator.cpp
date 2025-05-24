/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Maze Generator                                      *
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
struct shuffle final : public exprtk::igeneric_function<T>
{
   typedef typename exprtk::igeneric_function<T> igfun_t;
   typedef typename igfun_t::parameter_list_t    parameter_list_t;
   typedef typename igfun_t::generic_type        generic_type;
   typedef typename generic_type::vector_view    vector_t;

   using igfun_t::operator();

   shuffle()
   : exprtk::igeneric_function<T>("V")
   {
      std::random_device device;
      std::array<unsigned int,std::mt19937::state_size> seed;
      std::generate_n(seed.data(), seed.size(), std::ref(device));
      std::seed_seq seq(std::begin(seed), std::end(seed));
      generator.seed(seq);
   }

   inline T operator() (parameter_list_t parameters) override
   {
      vector_t vec(parameters[0]);
      std::shuffle(std::begin(vec), std::end(vec), generator);
      return T(1);
   }

   std::mt19937 generator;
   std::uniform_real_distribution<T> distribution{T(0),T(1)};
};

template <typename T>
void maze_generator()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   const T width  = T(40);
   const T height = T(30);

   const T N = T(1);
   const T E = T(2);
   const T S = T(3);
   const T W = T(4);

   std::vector<T> board           (static_cast<std::size_t>(width * height), T(0));
   std::vector<T> north_wall_board(static_cast<std::size_t>(width * height), T(0));
   std::vector<T> west_wall_board (static_cast<std::size_t>(width * height), T(0));

   shuffle<T> shuffle;

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;

   symbol_table.add_constant( "width"            , width           );
   symbol_table.add_constant( "height"           , height          );
   symbol_table.add_constant( "N"                , N               );
   symbol_table.add_constant( "E"                , E               );
   symbol_table.add_constant( "S"                , S               );
   symbol_table.add_constant( "W"                , W               );

   symbol_table.add_vector  ( "board"            , board           );
   symbol_table.add_vector  ( "north_wall_board" , north_wall_board);
   symbol_table.add_vector  ( "west_wall_board"  , west_wall_board );

   symbol_table.add_function( "shuffle"          , shuffle         );
   symbol_table.add_function( "print_digit"      ,
                             [](T v) ->T
                             { printf(" %d ",static_cast<int>(v)); return 0; });

   symbol_table.add_package (io_package);

   compositor_t compositor(symbol_table);

   compositor.load_variables(true);
   compositor.load_vectors  (true);

   compositor.add(
      function_t("get_board")
      .vars("x", "y")
      .expression
      ( " board[y * width + x]; " ));

   compositor.add(
      function_t("set_board")
      .vars("x", "y", "value")
      .expression
      ( " board[y * width + x] := value; " ));

   compositor.add(
      function_t("get_nwall_board")
      .vars("x", "y")
      .expression
      ( " north_wall_board[y * width + x]; " ));

   compositor.add(
      function_t("get_wwall_board")
      .vars("x", "y")
      .expression
      ( " west_wall_board[y * width + x]; " ));

   compositor.add(
      function_t("set_nwall_board")
      .vars("x", "y", "value")
      .expression
      ( " north_wall_board[y * width + x] := value; " ));

   compositor.add(
      function_t("set_wwall_board")
      .vars("x", "y", "value")
      .expression
      ( " west_wall_board[y * width + x] := value; " ));

   compositor.add(
      function_t("display_maze")
      .expression
      (
         " for (var x := 0; x < width; x += 1)                               "
         " {                                                                 "
         "    print('+---');                                                 "
         " };                                                                "
         "                                                                   "
         " println('+');                                                     "
         "                                                                   "
         " for (var y := 0; y < height; y += 1)                              "
         " {                                                                 "
         "    for (var x := 0; x < width; x += 1)                            "
         "    {                                                              "
         "       var segment := get_wwall_board(x,y) == W ? '    ' : '|   '; "
         "       print(segment);                                             "
         "    };                                                             "
         "                                                                   "
         "    println('|');                                                  "
         "                                                                   "
         "    for (var x := 0; x < width; x += 1)                            "
         "    {                                                              "
         "       var segment := get_nwall_board(x,y) == N ? '+   ' : '+---'; "
         "       print(segment);                                             "
         "    };                                                             "
         "                                                                   "
         "    println('+');                                                  "
         " };                                                                "
         "                                                                   "
         " println();                                                        "
      ));

   compositor.add(
      function_t("generate_maze")
      .vars("x","y")
      .expression
      (
         " set_board(x, y, 1);                                                  "
         "                                                                      "
         " var x_move [4] := { -1,  0, +1,  0 };                                "
         " var y_move [4] := {  0, +1,  0, -1 };                                "
         " var heading[4] := {  W,  N,  E,  S };                                "
         " var moves  [4] := {  0,  1,  2,  3 };                                "
         "                                                                      "
         " shuffle(moves);                                                      "
         "                                                                      "
         " for (var i := 0; i < moves[]; i += 1)                                "
         " {                                                                    "
         "    var move  := moves[i];                                            "
         "    var new_x := x + x_move[move];                                    "
         "    var new_y := y + y_move[move];                                    "
         "                                                                      "
         "    if ((new_x < 0     ) or (new_y < 0      )) continue;              "
         "    if ((new_x >= width) or (new_y >= height)) continue;              "
         "                                                                      "
         "    if (get_board(new_x, new_y) == 0)                                 "
         "    {                                                                 "
         "       if (heading[move] == N) { set_nwall_board(x, y, N); };         "
         "       if (heading[move] == W) { set_wwall_board(x, y, W); };         "
         "                                                                      "
         "       if (heading[move] == S) { set_nwall_board(new_x, new_y, N); }; "
         "       if (heading[move] == E) { set_wwall_board(new_x, new_y, W); }; "
         "                                                                      "
         "       generate_maze(new_x,new_y);                                    "
         "    }                                                                 "
         " };                                                                   "
      ));

   const std::string maze_generator_program =
      " generate_maze(0, 0); "
      " display_maze();      ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(maze_generator_program,expression);

   expression.value();
}

int main()
{
   maze_generator<double>();
   return 0;
}

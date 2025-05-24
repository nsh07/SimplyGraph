/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Sudoku Solver                                       *
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


#include <string>

#include "exprtk.hpp"


template <typename T>
void sudoku_solver()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   std::vector<T> board =
   {
      0, 0, 0, 0, 0, 8, 2, 0, 5,
      0, 0, 0, 3, 2, 0, 0, 0, 8,
      0, 0, 0, 0, 0, 5, 6, 0, 0,
      0, 1, 0, 0, 4, 7, 0, 9, 3,
      0, 0, 0, 0, 0, 0, 0, 0, 0,
      8, 6, 0, 2, 3, 0, 0, 1, 0,
      0, 0, 6, 8, 0, 0, 0, 0, 0,
      5, 0, 0, 0, 7, 3, 0, 0, 0,
      4, 0, 3, 9, 0, 0, 0, 0, 0,
   };

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;
   symbol_table.add_vector  ("board", board);
   symbol_table.add_function("print_digit",
                             [](T v) ->T
                             { printf(" %d ",static_cast<int>(v)); return 0; });
   symbol_table.add_package (io_package);

   compositor_t compositor(symbol_table);

   compositor.load_variables(true);
   compositor.load_vectors  (true);

   compositor.add(
      function_t("get_board")
      .vars("col", "row")
      .expression
      ( " board[row * sqrt(board[]) + col]; " ));

   compositor.add(
      function_t("set_board")
      .vars("col", "row", "value")
      .expression
      ( " board[row * sqrt(board[]) + col] := value; " ));

   compositor.add(
      function_t("display_board")
      .expression
      (
         " const var n := sqrt(board[]);                             "
         "                                                           "
         " for (var row := 0; row < n; row += 1)                     "
         " {                                                         "
         "    for (var col := 0; col < n; col += 1)                  "
         "    {                                                      "
         "       print_digit(get_board(col,row));                    "
         "                                                           "
         "       if ((col > 0) and (col < 8) and (col + 1) % 3 == 0) "
         "       {                                                   "
         "          print('|')                                       "
         "       }                                                   "
         "    };                                                     "
         "                                                           "
         "    println('');                                           "
         "                                                           "
         "    if ((row > 0) and (row < 8) and (row + 1) % 3 == 0)    "
         "    {                                                      "
         "       println('---------+---------+----------');          "
         "    }                                                      "
         " }                                                         "
      ));

   compositor.add(
      function_t("is_valid")
      .vars("col", "row", "num")
      .expression
      (
         " const var n := sqrt(board[]);                              "
         "                                                            "
         " for (var i := 0; i < n; i += 1)                            "
         " {                                                          "
         "    if (                                                    "
         "         (get_board(col, i  ) == num) or                    "
         "         (get_board(i  , row) == num)                       "
         "       )                                                    "
         "    {                                                       "
         "       return [false];                                      "
         "    }                                                       "
         " };                                                         "
         "                                                            "
         " var subgrid_row := row - floor(row % 3);                   "
         " var subgrid_col := col - floor(col % 3);                   "
         "                                                            "
         " for (var i := subgrid_row; i < subgrid_row + 3; i += 1)    "
         " {                                                          "
         "    for (var j := subgrid_col; j < subgrid_col + 3; j += 1) "
         "    {                                                       "
         "       if (get_board(j,i) == num)                           "
         "       {                                                    "
         "          return [false];                                   "
         "       }                                                    "
         "    }                                                       "
         " };                                                         "
         "                                                            "
         " return [true];                                             "
      ));

   compositor.add(
      function_t("solve_sudoku")
      .expression
      (
         " const var n := sqrt(board[]);                "
         "                                              "
         " for (var row := 0; row < n; row += 1)        "
         " {                                            "
         "    for (var col := 0; col < n; col += 1)     "
         "    {                                         "
         "       if (get_board(col, row) != 0)          "
         "       {                                      "
         "          continue;                           "
         "       }                                      "
         "                                              "
         "       for (var num := 1; num <= 9; num += 1) "
         "       {                                      "
         "          if (not(is_valid(col, row, num)))   "
         "          {                                   "
         "             continue;                        "
         "          }                                   "
         "                                              "
         "          set_board(col, row, num);           "
         "                                              "
         "          if (solve_sudoku())                 "
         "          {                                   "
         "             return [true];                   "
         "          };                                  "
         "                                              "
         "          set_board(col, row, 0);             "
         "       };                                     "
         "                                              "
         "       return [false];                        "
         "    }                                         "
         " };                                           "
         "                                              "
         " return [true];                               "
      ));

   const std::string sudoku_solver_program =
      " if (solve_sudoku())                           "
      " {                                             "
      "    println('Puzzle Solved!');                 "
      " }                                             "
      " else                                          "
      " {                                             "
      "    println('Error: Failed to solve puzzle.'); "
      " };                                            "
      "                                               "
      " display_board();                              ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(sudoku_solver_program,expression);

   expression.value();
}

int main()
{
   sudoku_solver<double>();
   return 0;
}

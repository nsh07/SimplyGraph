/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk N-Queens Problem                                    *
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

   const T n = T(15);
   std::vector<T> board(static_cast<std::size_t>(n), T(- 1));

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;
   symbol_table.add_constant("n"     , n    );
   symbol_table.add_vector  ("board" , board);
   symbol_table.add_package (io_package);

   compositor_t compositor(symbol_table);

   compositor.load_variables(true);
   compositor.load_vectors  (true);

   compositor.add(
      function_t("display_board")
      .expression
      (
         " for (var row := 0; row < n; row += 1)             "
         " {                                                 "
         "    for (var col := 0; col < n; col += 1)          "
         "    {                                              "
         "       print((board[row] == col) ? ' Q ' : ' . '); "
         "    };                                             "
         "                                                   "
         "    println('');                                   "
         " }                                                 "
      ));

   compositor.add(
      function_t("is_valid")
      .vars("row", "col")
      .expression
      (
         " for (var i := 0; i < row; i += 1)             "
         " {                                             "
         "    if (                                       "
         "         (board[i] == col) or                  "
         "         (abs(board[i] - col) == abs(i - row)) "
         "       )                                       "
         "    {                                          "
         "       return [false];                         "
         "    }                                          "
         " };                                            "
         "                                               "
         " return [true];                                "
      ));

   compositor.add(
      function_t("solve_nqueens")
      .var("row")
      .expression
      (
         " if (row == n)                         "
         " {                                     "
         "     return [true];                    "
         " };                                    "
         "                                       "
         " for (var col := 0; col < n; col += 1) "
         " {                                     "
         "    if (is_valid(row, col))            "
         "    {                                  "
         "       board[row] := col;              "
         "                                       "
         "       if (solve_nqueens(row + 1))     "
         "       {                               "
         "           return [true];              "
         "       }                               "
         "    }                                  "
         " };                                    "
         "                                       "
         " return [false];                       "
      ));

   const std::string sudoku_solver_program =
      " if (solve_nqueens(0))                                  "
      " {                                                      "
      "    println('N-Queens Puzzle Solved!');                 "
      " }                                                      "
      " else                                                   "
      " {                                                      "
      "    println('Error: Failed to solve n-queens puzzle.'); "
      " };                                                     "
      "                                                        "
      " display_board();                                       ";

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

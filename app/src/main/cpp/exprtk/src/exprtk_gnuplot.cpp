/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk GNUPlot Example                                     *
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
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "exprtk.hpp"


class exprtk_gnuplot_fx
{
public:

   exprtk_gnuplot_fx()
   : min_x_(0.0)
   , max_x_(0.0)
   , min_y_(0.0)
   , max_y_(0.0)
   {}

   exprtk_gnuplot_fx& set_title(const std::string& title)
   {
      title_ = title;
      return *this;
   }

   exprtk_gnuplot_fx& set_domain(const double min_x, const double max_x)
   {
      min_x_ = min_x;
      max_x_ = max_x;
      return *this;
   }

   exprtk_gnuplot_fx& set_expression(const std::string& expression)
   {
      expression_ = expression;
      return *this;
   }

   bool plot()
   {
      if (!generate_data())
         return false;
      else
         return generate_gp_script();
   }

private:

   bool generate_gp_script()
   {
      std::ofstream stream("plot.gp");

      if (!stream)
      {
         return false;
      }

      stream << "set term png\n";
      stream << "set output 'plot.png'\n";
      stream << "set xrange[" << min_x_ << ":" << max_x_ <<"]\n";
      stream << "set yrange[" << min_y_ << ":" << max_y_ <<"]\n";
      stream << "set xzeroaxis\n";
      stream << "set yzeroaxis\n";
      stream << "plot 'data.dat' using 1:2:(1.0) smooth unique title '" << title_ <<"'\n";

      return true;
   }

   bool generate_data()
   {
      typedef exprtk::symbol_table<double> symbol_table_t;
      typedef exprtk::expression<double>   expression_t;
      typedef exprtk::parser<double>       parser_t;

      double x = 0.0;

      symbol_table_t symbol_table;
      symbol_table.add_constants();
      symbol_table.add_variable("x",x);

      expression_t expression;
      expression.register_symbol_table(symbol_table);

      parser_t parser;

      if (!parser.compile(expression_,expression))
      {
         return false;
      }

      std::ofstream stream("data.dat");

      if (!stream)
      {
         return false;
      }

      min_y_ = +std::numeric_limits<double>::max();
      max_y_ = -std::numeric_limits<double>::max();

      stream << std::setprecision(10);

      const double increment = std::min(0.00005,std::abs(max_x_ - min_x_) / 1000.0);

      for (x = min_x_; x <= max_x_; x += increment)
      {
         const double y = expression.value();

              if (y < min_y_) min_y_ = y;
         else if (y > max_y_) max_y_ = y;

         stream << x << "\t" << y << "\n";
      }

      const double diff_y  = std::abs(max_y_ - min_y_);
      const double perc7_5 = diff_y * 0.075; //7.5%

      min_y_ -= perc7_5;
      max_y_ += perc7_5;

      return true;
   }

   std::string title_;
   std::string expression_;
   double      min_x_;
   double      max_x_;
   double      min_y_;
   double      max_y_;
};

int main()
{
   exprtk_gnuplot_fx plotter;

   plotter
      .set_expression("clamp(-1.0,sin(2 * pi * x) + cos(x / 2 * pi),+1.0)")
      .set_domain(-5,+5)
      .set_title("ExprTk GNUPlot Example");

   plotter.plot();

   return 0;
}


/*
   Build and Run:
   1. c++ -ansi -pedantic-errors -Wall -Wextra -Werror -Wno-long-long -O3 -DNDEBUG -o exprtk_gnuplot exprtk_gnuplot.cpp -lstdc++
   2. ./exprtk_gnuplot
   3. gnuplot plot.gp
*/

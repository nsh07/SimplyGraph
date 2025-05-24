/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk GNUPlot Multi-Curve Example                         *
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


class exprtk_gnuplot_fx;

class exprtk_fx_curve
{
public:

   friend class exprtk_gnuplot_fx;

   exprtk_fx_curve& set_title(const std::string& title)
   {
      title_ = title;
      return *this;
   }

   exprtk_fx_curve& set_domain(const double min_x, const double max_x)
   {
      min_x_ = min_x;
      max_x_ = max_x;
      return *this;
   }

   exprtk_fx_curve& set_expression(const std::string& expression)
   {
      expression_ = expression;
      return *this;
   }

private:

   std::string title_;
   std::string expression_;
   double      min_x_;
   double      max_x_;
   double      min_y_;
   double      max_y_;
};

class exprtk_gnuplot_fx
{
public:

   bool plot()
   {
      for (std::size_t i = 0; i < curve_list_.size(); ++i)
      {
         if (!generate_data(i,curve_list_[i]))
            return false;
      }

      return generate_gp_script();
   }

   exprtk_gnuplot_fx& add_curve(const exprtk_fx_curve& curve)
   {
      curve_list_.push_back(curve);
      return *this;
   }

private:

   bool generate_gp_script()
   {
      std::ofstream stream("plot.gp");

      if (!stream)
      {
         return false;
      }

      double min_x = +std::numeric_limits<double>::max();
      double max_x = -std::numeric_limits<double>::max();
      double min_y = +std::numeric_limits<double>::max();
      double max_y = -std::numeric_limits<double>::max();

      for (std::size_t i = 0; i < curve_list_.size(); ++i)
      {
         exprtk_fx_curve& curve = curve_list_[i];

         if (curve.min_x_ < min_x) min_x = curve.min_x_;
         if (curve.max_x_ > max_x) max_x = curve.max_x_;
         if (curve.min_y_ < min_y) min_y = curve.min_y_;
         if (curve.max_y_ > max_y) max_y = curve.max_y_;
      }

      stream << "set term png\n";
      stream << "set output 'plot.png'\n";
      stream << "set xrange[" << min_x << ":" << max_x <<"]\n";
      stream << "set yrange[" << min_y << ":" << max_y <<"]\n";
      stream << "set xzeroaxis\n";
      stream << "set yzeroaxis\n";
      stream << "plot \\\n";

      for (std::size_t i = 0; i < curve_list_.size(); ++i)
      {
         stream << "'data.dat" << i << "' using 1:2:(1.0) smooth unique title '" << curve_list_[i].title_;
         stream << (((i + 1) < curve_list_.size()) ? "',\\\n"  : "'\n");
      }

      return true;
   }

   bool generate_data(const std::size_t index, exprtk_fx_curve& curve)
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

      if (!parser.compile(curve.expression_,expression))
      {
         return false;
      }

      const std::string file_name = std::string("data.dat") + (char)('0' + index);

      std::ofstream stream(file_name.c_str());

      if (!stream)
      {
         return false;
      }

      curve.min_y_ = +std::numeric_limits<double>::max();
      curve.max_y_ = -std::numeric_limits<double>::max();

      stream << std::setprecision(10);

      const double increment = std::min(0.00005,std::abs(curve.max_x_ - curve.min_x_) / 1000.0);

      for (x = curve.min_x_; x <= curve.max_x_; x += increment)
      {
         const double y = expression.value();

              if (y < curve.min_y_) curve.min_y_ = y;
         else if (y > curve.max_y_) curve.max_y_ = y;

         stream << x << "\t" << y << "\n";
      }

      const double diff_y  = std::abs(curve.max_y_ - curve.min_y_);
      const double perc7_5 = diff_y * 0.075; //7.5%

      curve.min_y_ -= perc7_5;
      curve.max_y_ += perc7_5;

      return true;
   }

   std::deque<exprtk_fx_curve> curve_list_;
};

int main()
{
   exprtk_gnuplot_fx plotter;

   plotter
   .add_curve(
      exprtk_fx_curve()
         .set_expression("sin(2 * pi * x) + cos(x / 2 * pi)")
         .set_domain(-5,+5)
         .set_title("ExprTk Curve 1"))

   .add_curve(
      exprtk_fx_curve()
         .set_expression("clamp(-1.0,sin(2 * pi * x) + cos(x / 2 * pi),+1.0)")
         .set_domain(-5,+5)
         .set_title("ExprTk Curve 2"));

   plotter.plot();

   return 0;
}


/*
   Build and Run:
   1. c++ -pedantic-errors -Wall -Wextra -Werror -Wno-long-long -O3 -DNDEBUG -o exprtk_gnuplot_multi exprtk_gnuplot_multi.cpp -lstdc++
   2. ./exprtk_gnuplot_multi
   3. gnuplot plot.gp
*/

/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk CPU Primer                                          *
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
bool pgo_primer()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   static const std::string expression_list[] =
      {
         "(y + x)",
         "2 * (y + x)",
         "(2 * y + 2 * x)",
         "(y + x / y) * (x - y / x)",
         "x / ((x + y) * (x - y)) / y",
         "1 - ((x * y) + (y / x)) - 3",
         "sin(2 * x) + cos(pi / y)",
         "1 - sin(2 * x) + cos(pi / y)",
         "sqrt(1 - sin(2 * x) + cos(pi / y) / 3)",
         "(x^2 / sin(2 * pi / y)) -x / 2",
         "x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y",
         "clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
         "iclamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
         "max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))",
         "if(avg(x,y) <= x + y, x - y, x * y) + 2 * pi / x",
         "1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^4 - 5.5x^5 + 6.6y^6 - 7.7x^27 + 8.8y^55",
         "(yy + xx)",
         "2 * (yy + xx)",
         "(2 * yy + 2 * xx)",
         "(yy + xx / yy) * (xx - yy / xx)",
         "xx / ((xx + yy) * (xx - yy)) / yy",
         "1 - ((xx * yy) + (yy / xx)) - 3",
         "sin(2 * xx) + cos(pi / yy)",
         "1 - sin(2 * xx) + cos(pi / yy)",
         "sqrt(1 - sin(2 * xx) + cos(pi / yy) / 3)",
         "(xx^2 / sin(2 * pi / yy)) -xx / 2",
         "xx + (cos(yy - sin(2 / xx * pi)) - sin(xx - cos(2 * yy / pi))) - yy",
         "clamp(-1.0, sin(2 * pi * xx) + cos(yy / 2 * pi), +1.0)",
         "max(3.33, min(sqrt(1 - sin(2 * xx) + cos(pi / yy) / 3), 1.11))",
         "if(avg(xx,yy) <= xx + yy, xx - yy, xx * yy) + 2 * pi / xx",
         "1.1xx^1 + 2.2yy^2 - 3.3xx^3 + 4.4yy^4 - 5.5xx^5 + 6.6yy^6 - 7.7xx^27 + 8.8yy^55",
         "(1.1*(2.2*(3.3*(4.4*(5.5*(6.6*(7.7*(8.8*(9.9+x)))))))))",
         "(((((((((x+9.9)*8.8)*7.7)*6.6)*5.5)*4.4)*3.3)*2.2)*1.1)",
         "(x + y) * z", "x + (y * z)", "(x + y) * 7", "x + (y * 7)",
         "(x + 7) * y", "x + (7 * y)", "(7 + x) * y", "7 + (x * y)",
         "(2 + x) * 3", "2 + (x * 3)", "(2 + 3) * x", "2 + (3 * x)",
         "(x + 2) * 3", "x + (2 * 3)",
         "(x + y) * (z / w)", "(x + y) * (z / 7)", "(x + y) * (7 / z)", "(x + 7) * (y / z)",
         "(7 + x) * (y / z)", "(2 + x) * (y / z)", "(x + 2) * (y / 3)", "(2 + x) * (y / 3)",
         "(x + 2) * (3 / y)", "x + (y * (z / w))", "x + (y * (z / 7))", "x + (y * (7 / z))",
         "x + (7 * (y / z))", "7 + (x * (y / z))", "2 + (x * (3 / y))", "x + (2 * (y / 4))",
         "2 + (x * (y / 3))", "x + (2 * (3 / y))",
         "x + ((y * z) / w)", "x + ((y * z) / 7)", "x + ((y * 7) / z)", "x + ((7 * y) / z)",
         "7 + ((y * z) / w)", "2 + ((x * 3) / y)", "x + ((2 * y) / 3)", "2 + ((x * y) / 3)",
         "x + ((2 * 3) / y)", "(((x + y) * z) / w)",
         "(((x + y) * z) / 7)", "(((x + y) * 7) / z)", "(((x + 7) * y) / z)", "(((7 + x) * y) / z)",
         "(((2 + x) * 3) / y)", "(((x + 2) * y) / 3)", "(((2 + x) * y) / 3)", "(((x + 2) * 3) / y)",
         "((x + (y * z)) / w)", "((x + (y * z)) / 7)", "((x + (y * 7)) / y)", "((x + (7 * y)) / z)",
         "((7 + (x * y)) / z)", "((2 + (x * 3)) / y)", "((x + (2 * y)) / 3)", "((2 + (x * y)) / 3)",
         "((x + (2 * 3)) / y)",
         "(xx + yy) * zz", "xx + (yy * zz)",
         "(xx + yy) * 7", "xx + (yy * 7)",
         "(xx + 7) * yy", "xx + (7 * yy)",
         "(7 + xx) * yy", "7 + (xx * yy)",
         "(2 + x) * 3", "2 + (x * 3)",
         "(2 + 3) * x", "2 + (3 * x)",
         "(x + 2) * 3", "x + (2 * 3)",
         "(xx + yy) * (zz / ww)", "(xx + yy) * (zz / 7)",
         "(xx + yy) * (7 / zz)", "(xx + 7) * (yy / zz)",
         "(7 + xx) * (yy / zz)", "(2 + xx) * (yy / zz)",
         "(xx + 2) * (yy / 3)", "(2 + xx) * (yy / 3)",
         "(xx + 2) * (3 / yy)", "xx + (yy * (zz / ww))",
         "xx + (yy * (zz / 7))", "xx + (yy * (7 / zz))",
         "xx + (7 * (yy / zz))", "7 + (xx * (yy / zz))",
         "2 + (xx * (3 / yy))", "xx + (2 * (yy / 4))",
         "2 + (xx * (yy / 3))", "xx + (2 * (3 / yy))",
         "xx + ((yy * zz) / ww)", "xx + ((yy * zz) / 7)",
         "xx + ((yy * 7) / zz)", "xx + ((7 * yy) / zz)",
         "7 + ((yy * zz) / ww)", "2 + ((xx * 3) / yy)",
         "xx + ((2 * yy) / 3)", "2 + ((xx * yy) / 3)",
         "xx + ((2 * 3) / yy)", "(((xx + yy) * zz) / ww)",
         "(((xx + yy) * zz) / 7)", "(((xx + yy) * 7) / zz)",
         "(((xx + 7) * yy) / zz)", "(((7 + xx) * yy) / zz)",
         "(((2 + xx) * 3) / yy)", "(((xx + 2) * yy) / 3)",
         "(((2 + xx) * yy) / 3)", "(((xx + 2) * 3) / yy)",
         "((xx + (yy * zz)) / ww)", "((xx + (yy * zz)) / 7)",
         "((xx + (yy * 7)) / yy)", "((xx + (7 * yy)) / zz)",
         "((7 + (xx * yy)) / zz)", "((2 + (xx * 3)) / yy)",
         "((xx + (2 * yy)) / 3)", "((2 + (xx * yy)) / 3)",
         "((xx + (2 * 3)) / yy)"
      };

   static const std::size_t expression_list_size = sizeof(expression_list) / sizeof(std::string);

   T  x = T(0);
   T  y = T(0);
   T  z = T(0);
   T  w = T(0);
   T xx = T(0);
   T yy = T(0);
   T zz = T(0);
   T ww = T(0);

   symbol_table_t symbol_table;
   symbol_table.add_constants();
   symbol_table.add_variable( "x", x);
   symbol_table.add_variable( "y", y);
   symbol_table.add_variable( "z", z);
   symbol_table.add_variable( "w", w);
   symbol_table.add_variable("xx",xx);
   symbol_table.add_variable("yy",yy);
   symbol_table.add_variable("zz",zz);
   symbol_table.add_variable("ww",ww);

   typedef typename std::deque<expression_t> expr_list_t;
   expr_list_t expr_list;

   const std::size_t rounds = 50;

   {
      for (std::size_t r = 0; r < rounds; ++r)
      {
         expr_list.clear();
         parser_t parser;

         for (std::size_t i = 0; i < expression_list_size; ++i)
         {
            expression_t expression;
            expression.register_symbol_table(symbol_table);

            if (!parser.compile(expression_list[i],expression))
            {
               return false;
            }

            expr_list.push_back(expression);
         }
      }
   }

   struct execute
   {
      static inline T process(T& x, T& y, expression_t& expression)
      {
         static const T lower_bound = T(-20);
         static const T upper_bound = T(+20);
         static const T delta       = T(0.1);

         T total = T(0);

         for (x = lower_bound; x <= upper_bound; x += delta)
         {
            for (y = lower_bound; y <= upper_bound; y += delta)
            {
               total += expression.value();
            }
         }

         return total;
      }
   };

   for (std::size_t i = 0; i < expr_list.size(); ++i)
   {
      execute::process( x,  y, expr_list[i]);
      execute::process(xx, yy, expr_list[i]);
   }

   {
      using namespace exprtk;

      for (std::size_t i = 0; i < 10000; ++i)
      {
         const T v = T(123.456 + i);

         if (details::is_true(details::numeric::nequal(details::numeric::fast_exp<T, 1>::result(v),details::numeric::pow(v,T(1)))))
            return false;

         #define else_stmt(N)                                                                                                           \
         else if (details::is_true(details::numeric::nequal(details::numeric::fast_exp<T,N>::result(v),details::numeric::pow(v,T(N))))) \
            return false;                                                                                                               \

         else_stmt( 2) else_stmt( 3) else_stmt( 4) else_stmt( 5)
         else_stmt( 6) else_stmt( 7) else_stmt( 8) else_stmt( 9)
         else_stmt(10) else_stmt(11) else_stmt(12) else_stmt(13)
         else_stmt(14) else_stmt(15) else_stmt(16) else_stmt(17)
         else_stmt(18) else_stmt(19) else_stmt(20) else_stmt(21)
         else_stmt(22) else_stmt(23) else_stmt(24) else_stmt(25)
         else_stmt(26) else_stmt(27) else_stmt(28) else_stmt(29)
         else_stmt(30) else_stmt(31) else_stmt(32) else_stmt(33)
         else_stmt(34) else_stmt(35) else_stmt(36) else_stmt(37)
         else_stmt(38) else_stmt(39) else_stmt(40) else_stmt(41)
         else_stmt(42) else_stmt(43) else_stmt(44) else_stmt(45)
         else_stmt(46) else_stmt(47) else_stmt(48) else_stmt(49)
         else_stmt(50) else_stmt(51) else_stmt(52) else_stmt(53)
         else_stmt(54) else_stmt(55) else_stmt(56) else_stmt(57)
         else_stmt(58) else_stmt(59) else_stmt(60) else_stmt(61)
      }
   }

   return true;
}

int main()
{
   pgo_primer<double>();
   return 0;
}

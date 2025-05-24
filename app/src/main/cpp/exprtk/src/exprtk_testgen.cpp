/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Evaluated Test Expression Generator                        *
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
#include <fstream>
#include <string>

#include "exprtk.hpp"


/*
  Example usage:
  echo "x:=11.1237567810735" >  base0.tmp
  echo "y:=13.1239567810735" >> base0.tmp
  echo "z:=15.1233567810735" >> base0.tmp
  echo "w:=19.1235567810735" >> base0.tmp
  ./exprtk_exprgen > base1.tmp
  cat base1.tmp | sed 's/[{(,)}]//g' | awk ' !x[$0]++' > base2.tmp
  cat base0.tmp base1.tmp base2.tmp > exprtk_functional_ext_test.tmp
  ./exprtk_testgen exprtk_functional_ext_test.tmp > exprtk_functional_ext_test_`date "+%Y%m%d%H%M%S"`.txt

*/


template <typename Allocator,
          template <typename,typename> class Sequence>
inline std::size_t load_expressions(const std::string& file_name,
                                    Sequence<std::string,Allocator>& sequence)
{
   std::ifstream stream(file_name.c_str());
   if (!stream) return 0;
   std::string buffer;
   buffer.reserve(1024);
   std::size_t line_count = 0;
   while (std::getline(stream,buffer))
   {
      if (buffer.empty())
         continue;
      else if ('#' == buffer[0])
         continue;
      ++line_count;
      sequence.push_back(buffer);
   }

   return line_count;
}

template <typename T>
inline bool test_gen(const std::string& expr_file)
{
   typedef exprtk::expression<T> expression_t;

   T x = T(0);
   T y = T(0);
   T z = T(0);
   T w = T(0);

   exprtk::polynomial<T, 1> poly01;
   exprtk::polynomial<T, 2> poly02;
   exprtk::polynomial<T, 3> poly03;
   exprtk::polynomial<T, 4> poly04;
   exprtk::polynomial<T, 5> poly05;
   exprtk::polynomial<T, 6> poly06;
   exprtk::polynomial<T, 7> poly07;
   exprtk::polynomial<T, 8> poly08;
   exprtk::polynomial<T, 9> poly09;
   exprtk::polynomial<T,10> poly10;
   exprtk::polynomial<T,11> poly11;
   exprtk::polynomial<T,12> poly12;

   exprtk::symbol_table<T> symbol_table;
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);
   symbol_table.add_variable("z",z);
   symbol_table.add_variable("w",w);
   symbol_table.add_function("poly01", poly01);
   symbol_table.add_function("poly02", poly02);
   symbol_table.add_function("poly03", poly03);
   symbol_table.add_function("poly04", poly04);
   symbol_table.add_function("poly05", poly05);
   symbol_table.add_function("poly06", poly06);
   symbol_table.add_function("poly07", poly07);
   symbol_table.add_function("poly08", poly08);
   symbol_table.add_function("poly09", poly09);
   symbol_table.add_function("poly10", poly10);
   symbol_table.add_function("poly11", poly11);
   symbol_table.add_function("poly12", poly12);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   exprtk::parser<T> parser;

   std::deque<std::string> expr_str_list;

   if (0 == load_expressions(expr_file,expr_str_list))
   {
      return true;
   }

   std::deque<exprtk::expression<T> > expression_list;

   bool load_success = true;

   for (std::size_t i = 0; i < expr_str_list.size(); ++i)
   {
      exprtk::expression<T> current_expression;

      current_expression.register_symbol_table(symbol_table);

      if (!parser.compile(expr_str_list[i],current_expression))
      {
         load_success = false;
         printf("test_gen() - Error: %s   Expression: %s\n",
                parser.error().c_str(),
                expr_str_list[i].c_str());
      }
      else
         expression_list.push_back(current_expression);
   }

   if (!load_success)
      return false;

   for (std::size_t i = 0; i < expression_list.size(); ++i)
   {
      T result = expression_list[i].value();

      printf("equal((%050.30f),(%s))\n",
             result,
             expr_str_list[i].c_str());
   }

   return true;
}

int main(int argc, char* argv[])
{
   if (argc != 2)
   {
      printf("usage: exprtk_testgen <file name>\n");
      return 1;
   }

   const std::string file_name = argv[1];
   test_gen<double>(file_name);

   return 0;
}

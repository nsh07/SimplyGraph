/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Basic Arithmetic Expression Parser And Evaluator           *
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


template <typename T, T Process(const unsigned char)>
struct char_process : public exprtk::igeneric_function<T>
{
   typedef typename exprtk::igeneric_function<T> igfun_t;
   typedef typename igfun_t::parameter_list_t    parameter_list_t;
   typedef typename igfun_t::generic_type        generic_type;
   typedef typename generic_type::string_view    string_t;

   using exprtk::igeneric_function<T>::operator();

   char_process()
   : exprtk::igeneric_function<T>("S")
   {}

   inline T operator()(parameter_list_t parameters)
   {
      const unsigned char c = string_t(parameters[0])[0];
      return Process(c);
   }
};

template <typename T>
T is_digit_func(const unsigned char c)
{
   return (('0' <= c) && (c <= '9')) ? T(1) : T(0);
}

template <typename T>
T is_operator_func(const unsigned char c)
{
   return ('+' == c) || ('-' == c) ||
          ('*' == c) || ('/' == c) ;
}

template <typename T>
T to_num_func(const unsigned char c)
{
   return static_cast<T>(c - '0');
}

template <typename T>
void basic_arithmetic_parser_evaluator()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string arithmetic_parser_evaluator_program =
      " var curr_val := 0;                                "
      " var prev_val := 0;                                "
      " var result   := 0;                                "
      " var op       := '+';                              "
      "                                                   "
      " for (var i := 0; i < expression[]; i += 1)        "
      " {                                                 "
      "    var c := expression[i : i + 1];                "
      "                                                   "
      "    if (is_digit(c))                               "
      "    {                                              "
      "       curr_val := curr_val * 10 + to_num(c);      "
      "    };                                             "
      "                                                   "
      "    if (is_opr(c) or (i == expression[] - 1))      "
      "    {                                              "
      "       prev_val :=                                 "
      "          switch                                   "
      "          {                                        "
      "             case op == '+' :                      "
      "              ~{                                   "
      "                   result += prev_val;             "
      "                   curr_val;                       "
      "               };                                  "
      "                                                   "
      "             case op == '-' :                      "
      "              ~{                                   "
      "                  result += prev_val;              "
      "                  -curr_val;                       "
      "               };                                  "
      "                                                   "
      "             case op == '*' : prev_val * curr_val; "
      "             case op == '/' : prev_val / curr_val; "
      "          };                                       "
      "                                                   "
      "       curr_val := 0;                              "
      "       op       := c;                              "
      "    }                                              "
      " };                                                "
      "                                                   "
      " result += prev_val;                               "
      "                                                   "
      " println(result, ' = ', expression);               "
      "                                                   ";

   std::string arithmetic_expression;

   char_process<T,is_digit_func<T> >    isdigit;
   char_process<T,to_num_func<T> >      tonum;
   char_process<T,is_operator_func<T> > isopr;
   exprtk::rtl::io::println<T>          println;

   symbol_table_t symbol_table;
   symbol_table.add_stringvar("expression", arithmetic_expression);
   symbol_table.add_function ("println"   , println              );
   symbol_table.add_function ("is_digit"  , isdigit              );
   symbol_table.add_function ("is_opr"    , isopr                );
   symbol_table.add_function ("to_num"    , tonum                );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(arithmetic_parser_evaluator_program, expression);

   /*
      Note: Expressions can be comprised of integers,
            basic arithmetic operators (+, -, *, /),
            and shall not contain parenthesises.
    */
   const std::vector<std::string> arithmetic_expressions =
   {
      "5 + 18 / 3",
      "4 + 3 * 15",
      "1 - 2 + 52 / 2 - 81 / 3",
   };

   for (const auto& expr : arithmetic_expressions)
   {
      arithmetic_expression = expr;
      expression.value();
   }
}

int main()
{
   basic_arithmetic_parser_evaluator<double>();
   return 0;
}

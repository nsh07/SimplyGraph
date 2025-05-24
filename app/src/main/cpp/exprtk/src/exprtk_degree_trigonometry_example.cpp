/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Trigonometry In Degrees Example                     *
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
struct sine_deg final : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   sine_deg() : exprtk::ifunction<T>(1) {}

   inline T operator()(const T& v) override
   {
      return std::sin((v * T(exprtk::details::numeric::constant::pi)) / T(180));
   }
};

template <typename T>
struct cosine_deg final : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   cosine_deg() : exprtk::ifunction<T>(1) {}

   inline T operator()(const T& v) override
   {
      return std::cos((v * T(exprtk::details::numeric::constant::pi)) / T(180));
   }
};

template <typename T>
void exprtk_degree_trigonometry_example()
{
   typedef exprtk::symbol_table<T>           symbol_table_t;
   typedef exprtk::expression<T>             expression_t;
   typedef exprtk::parser<T>                 parser_t;
   typedef typename parser_t::settings_store settings_t;

   sine_deg  <T> sine;
   cosine_deg<T> cosine;

   exprtk::rtl::io::package<T> io_package;

   symbol_table_t symbol_table;

   symbol_table.add_package          (io_package   );
   symbol_table.add_reserved_function("sin", sine  );
   symbol_table.add_reserved_function("cos", cosine);

   typedef typename parser_t::settings_store settings_t;

   parser_t parser;

   // Disable the ExprTk internal sine/cosine functions
   parser.settings()
      .disable_base_function(settings_t::e_bf_sin)
      .disable_base_function(settings_t::e_bf_cos);

   const std::string trigonometry_program =
      " println('sin(30) = ', sin(30)); "
      " println('cos(30) = ', cos(30)); "
      " println('sin(45) = ', sin(45)); "
      " println('cos(45) = ', cos(45)); "
      " println('sin(60) = ', sin(60)); "
      " println('cos(60) = ', cos(60)); ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser.compile(trigonometry_program,expression);

   expression.value();
}

int main()
{
   exprtk_degree_trigonometry_example<double>();
   return 0;
}

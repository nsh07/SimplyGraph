/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Truth Table Generator                               *
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


#include <cmath>
#include <cstdio>
#include <string>

#include "exprtk.hpp"


template <typename T>
void truth_table_generator(const std::string& boolean_expression)
{
   typedef exprtk::symbol_table<T>    symbol_table_t;
   typedef exprtk::expression<T>      expression_t;
   typedef exprtk::parser<T>          parser_t;
   typedef exprtk::parser_error::type err_t;
   typedef typename parser_t::settings_store settings_t;

   symbol_table_t symbol_table;

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   static const std::size_t compile_options =
      settings_t::e_replacer            +
      settings_t::e_joiner              +
      settings_t::e_numeric_check       +
      settings_t::e_bracket_check       +
      settings_t::e_sequence_check      +
      settings_t::e_strength_reduction  +
      settings_t::e_disable_usr_on_rsrvd;

   parser_t parser(settings_t(compile_options)
                     .disable_all_base_functions    ()
                     .disable_all_control_structures());

   parser.enable_unknown_symbol_resolver();

   parser.dec().collect_variables() = true;
   parser.dec().collect_functions() = true;

   if (!parser.compile(boolean_expression,expression))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             boolean_expression.c_str());

      for (std::size_t i = 0; i < parser.error_count(); ++i)
      {
         err_t error = parser.get_error(i);

         printf("Error: %02d Position: %02d "
                "Type: [%s] "
                "Message: %s "
                "Expression: %s\n",
                static_cast<int>(i),
                static_cast<int>(error.token.position),
                exprtk::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str(),
                boolean_expression.c_str());

         exprtk::parser_error::update_error(error,boolean_expression);

         printf("Error[%02d] at line: %d column: %d\n",
                static_cast<int>(i),
                static_cast<int>(error.line_no),
                static_cast<int>(error.column_no));
      }

      return;
   }

   typedef typename parser_t::dependent_entity_collector::symbol_t dec_symbol_t;

   std::deque<dec_symbol_t> symbol_list;

   parser.dec().symbols(symbol_list);

   if (symbol_list.empty())
   {
      printf("Error: No variables found to build truth table.\n");
      return;
   }

   for (std::size_t i = 0; i < symbol_list.size(); ++i)
   {
      if (exprtk::details::imatch(symbol_list[i].first,"not"))
      {
         symbol_list.erase(symbol_list.begin() + i);

         if (i >= symbol_list.size())
            break;
      }

      if (parser_t::e_st_function == symbol_list[i].second)
      {
         printf("Error: call to function '%s' not allowed.\n",symbol_list[i].first.c_str());
         return;
      }
   }

   if (symbol_list.size() > 32)
   {
      printf("Error: no more than 32 unique variables allowed.\n");
      return;
   }

   unsigned int upper_bound = 1 << symbol_list.size();

   const int index_width = static_cast<int>(std::ceil(std::log10(upper_bound)));

   // Print truth table header
   printf(" #%s ",std::string(index_width - 1,' ').c_str());

   for (std::size_t i = 0; i < symbol_list.size(); ++i)
   {
      printf("| %s ",symbol_list[i].first.c_str());
   }

   printf("| %s \n",boolean_expression.c_str());

   for (std::size_t i = 0; i < upper_bound; ++i)
   {
      for (std::size_t j = 0; j < symbol_list.size(); ++j)
      {
         symbol_table.get_variable(symbol_list[j].first)->ref() = T((i & (1 << (symbol_list.size() - j - 1))) ? 0 : 1);
      }

      const std::size_t result = static_cast<std::size_t>(expression.value());

      printf(" %*d ", index_width, static_cast<int>(i));

      for (std::size_t j = 0; j < symbol_list.size(); ++j)
      {
         printf("| %d ",static_cast<int>(symbol_table.get_variable(symbol_list[j].first)->value()));
      }

      printf("| %d \n", static_cast<int>(result));
   }
}

int main(int argc, char* argv[])
{
   if (2 != argc)
   {
      printf("usage: exprtk_truthtable_gen <expression>\n");
      return 1;
   }

   truth_table_generator<double>(argv[1]);

   return 0;
}

/*
   Examples:
   ./exprtk_truthtable_gen 'A and B or C or not(D)'
   ./exprtk_truthtable_gen '(A and not(B)) nor not(C xor D)'
   ./exprtk_truthtable_gen '(A nand not(B or C)) xor not(D xor E)'
*/

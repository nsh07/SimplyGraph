/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Logical Deduction Riddle                            *
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


/*

   The Broken Window Riddle

   A window has been broken, there are 5 suspects (Annie, Betsy, Chloe,
   Tori and Zoey). Each suspect is asked who they think broke the window.

   The following is each suspect's statement:

      Tori : It wasn't Zoey, It was Annie
      Annie: It wasn't Betsy, It wasn't Zoey
      Betsy: It was Zoey, It wasn't Tori
      Chloe: It was Betsy, It was Annie
      Zoey : It was Chloe, It wasn't Tori

   Given each of the suspects spoke only one lie and one truth, and only
   one person is responsible for the broken window:

   Who then was it that broke the window?

*/

#include <string>

#include "exprtk.hpp"


template <typename T>
void logical_deducation_riddle()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   symbol_table_t symbol_table;

   exprtk::rtl::io::println<T> println;

   symbol_table.add_function("println", println);

   compositor_t compositor(symbol_table);

   compositor.add(function_t()
      .name("constraint")
      .var("person_x").var("person_y")
      .expression
      (
         " person_x xor person_y; "
      ));

   const std::string logical_deducation_riddle_program =
      " const var number_of_culprits := 1;                                             "
      "                                                                                "
      " for (var Tori := false; Tori <= true; Tori += true)                            "
      " {                                                                              "
      "    for (var Annie := false; Annie <= true; Annie += true)                      "
      "    {                                                                           "
      "       for (var Betsy := false; Betsy <= true; Betsy += true)                   "
      "       {                                                                        "
      "          for (var Chloe := false; Chloe <= true; Chloe += true)                "
      "          {                                                                     "
      "             for (var Zoey := false; Zoey <= true; Zoey += true)                "
      "             {                                                                  "
      "                if (sum(Annie, Betsy, Chloe, Tori, Zoey) != number_of_culprits) "
      "                   continue;                                                    "
      "                                                                                "
      "                var solution :=                                                 "
      "                   constraint( not(Zoey)  , Annie     ) and /* Tori  */         "
      "                   constraint( not(Betsy) , not(Zoey) ) and /* Annie */         "
      "                   constraint( Zoey       , not(Tori) ) and /* Betsy */         "
      "                   constraint( Betsy      , Annie     ) and /* Chloe */         "
      "                   constraint( Chloe      , not(Tori) ) ;   /* Zoey  */         "
      "                                                                                "
      "                if (solution == true)                                           "
      "                {                                                               "
      "                   var culprit := '';                                           "
      "                   [*]                                                          "
      "                   {                                                            "
      "                      case Annie : culprit := 'Annie';                          "
      "                      case Betsy : culprit := 'Betsy';                          "
      "                      case Chloe : culprit := 'Chloe';                          "
      "                      case Tori  : culprit := 'Tori' ;                          "
      "                      case Zoey  : culprit := 'Zoey' ;                          "
      "                   };                                                           "
      "                                                                                "
      "                   println(culprit,' broke the window!');                       "
      "                }                                                               "
      "             }                                                                  "
      "          }                                                                     "
      "       }                                                                        "
      "    }                                                                           "
      " }                                                                              "
      "                                                                                ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(logical_deducation_riddle_program,expression);

   expression.value();
}

int main()
{
   logical_deducation_riddle<double>();
   return 0;
}

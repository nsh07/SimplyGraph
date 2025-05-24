/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Functional Test Expression Generator                       *
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


#include <iostream>
#include <string>
#include <deque>
#include <iterator>


/*

  Usage:
  ./exprtk_exprgen > base
  cat base | sed 's/[{(,)}]//g' | awk ' !x[$0]++' > base2
  cat base base2 > exprtk_functional_ext_test`date "+%Y%m%d%H%M%S"`.txt

*/


inline void gen_perm(const std::string& s,
                     const std::size_t n,
                     std::deque<std::string>& perm_list,
                     const std::string prefix = "")
{
   if (n == 0)
   {
      perm_list.push_back(prefix);
      return;
   }

   for (std::size_t i = 0; i < s.size(); ++i)
   {
      gen_perm(s, n - 1, perm_list,prefix + s[i]);
   }
}

int main()
{

   std::deque<std::string> e_3;
   std::deque<std::string> e_4;

   std::deque<std::string> e_3_comb;
   std::deque<std::string> e_4_comb;

   std::deque<std::string> e_43_comb;
   std::deque<std::string> e_34_comb;

   std::deque<std::string> e_434_comb;
   std::deque<std::string> e_343_comb;

   {
      std::deque<std::string> operator_list;
      gen_perm("+-*/", 2, operator_list);

      std::string expr[] =
         {
            "((xAy)Bz)",
            "(xA(yBz))",
            "((3.579Ay)Bz)",
            "(3.579A(yBz))",
            "((xA5.793)Bz)",
            "(xA(5.793Bz))",
            "((xAy)B7.935)",
            "(xA(yB7.935))",
            "((3.579A5.793)Bz)",
            "(3.579A(5.793Bz))",
            "((3.579Ay)B7.935)",
            "(3.579A(yB7.935))",
            "((xA5.793)B7.935)",
            "(xA(5.793B7.935))",
            "((3.579A5.793)B7.935)",
            "(3.579A(5.793B7.935))"
         };

      for (std::size_t i = 0; i < sizeof(expr) / sizeof(std::string); ++i)
      {
         std::size_t loc[] =
                     {
                       expr[i].find("A"),
                       expr[i].find("B")
                     };

         for (std::size_t j = 0; j < operator_list.size(); ++j)
         {
            std::string curr_expr = expr[i];
            for (std::size_t k = 0; k < sizeof(loc)/sizeof(std::size_t); ++k)
            {
               curr_expr[loc[k]] = operator_list[j][k];
            }

            //std::cout << curr_expr << "\n";
            e_3.push_back(curr_expr);
         }
      }
   }

   {
      std::deque<std::string> operator_list;
      gen_perm("+-*/", 3, operator_list);

      std::string expr[] =
      {
         "((xAy)B(zCw))",
         "((xA(yB(zCw))))",
         "((xA((yBz)Cw)))",
         "((((xAy)Bz)Cw))",
         "(((xA(yBz))Cw))",
         "((3.579Ay)B(zCw))",
         "((3.579A(yB(zCw))))",
         "((3.579A((yBz)Cw)))",
         "((((3.579Ay)Bz)Cw))",
         "(((3.579A(yBz))Cw))",
         "((xA5.793)B(zCw))",
         "((xA(5.793B(zCw))))",
         "((xA((5.793Bz)Cw)))",
         "((((xA5.793)Bz)Cw))",
         "(((xA(5.793Bz))Cw))",
         "((xAy)B(7.935Cw))",
         "((xA(yB(7.935Cw))))",
         "((xA((yB7.935)Cw)))",
         "((((xAy)B7.935)Cw))",
         "(((xA(yB7.935))Cw))",
         "((xAy)B(zC9.357))",
         "((xA(yB(zC9.357))))",
         "((xA((yBz)C9.357)))",
         "((((xAy)Bz)C9.357))",
         "(((xA(yBz))C9.357))",
         "((3.579A5.793)B(zCw))",
         "((3.579A(5.793B(zCw))))",
         "((3.579A((5.793Bz)Cw)))",
         "((((3.579A5.793)Bz)Cw))",
         "(((3.579A(5.793Bz))Cw))",
         "((3.579Ay)B(5.793Cw))",
         "((3.579A(yB(5.793Cw))))",
         "((3.579A((yB5.793)Cw)))",
         "((((3.579Ay)B5.793)Cw))",
         "(((3.579A(yB5.793))Cw))",
         "((3.579Ay)B(zC9.357))",
         "((3.579A(yB(zC9.357))))",
         "((3.579A((yBz)C9.357)))",
         "((((3.579Ay)Bz)C9.357))",
         "(((3.579A(yBz))C9.357))",
         "((xA5.793)B(7.935Cw))",
         "((xA(5.793B(7.935Cw))))",
         "((xA((5.793B7.935)Cw)))",
         "((((xA5.793)B7.935)Cw))",
         "(((xA(5.793B7.935))Cw))",
         "((xA5.793)B(zC9.357))",
         "((xA(5.793B(zC9.357))))",
         "((xA((5.793Bz)C9.357)))",
         "((((xA5.793)Bz)C9.357))",
         "(((xA(5.793Bz))C9.357))",
         "((xAy)B(7.935C9.357))",
         "((xA(yB(7.935C9.357))))",
         "((xA((yB7.935)C9.357)))",
         "((((xAy)B7.935)C9.357))",
         "(((xA(yB7.935))C9.357))",
         "((3.579A5.793)B(7.935C9.357))",
         "((3.579A(5.793B(7.935C9.357))))",
         "((3.579A((5.793B7.935)C9.357)))",
         "((((3.579A5.793)B7.935)C9.357))",
         "(((3.579A(5.793B7.935))C9.357))"
      };

      for (std::size_t i = 0; i < sizeof(expr) / sizeof(std::string); ++i)
      {
         std::size_t loc[] =
                     {
                       expr[i].find("A"),
                       expr[i].find("B"),
                       expr[i].find("C")
                     };

         for (std::size_t j = 0; j < operator_list.size(); ++j)
         {
            std::string curr_expr = expr[i];
            for (std::size_t k = 0; k < sizeof(loc)/sizeof(std::size_t); ++k)
            {
               curr_expr[loc[k]] = operator_list[j][k];
            }

            //std::cout << curr_expr << "\n";
            e_4.push_back(curr_expr);
         }
      }
   }

   {
      const std::string operators = "+-*/";
      std::size_t opr_index = 0;
      for (std::size_t i = 0; i < e_3.size(); ++i)
      {
         std::string curr_str = "(" + e_3[i] + operators[opr_index] + e_3[(i + 10)  % e_3.size()] + ")";
         e_3_comb.push_back(curr_str);
         opr_index = (opr_index + 1) % operators.size();
      }
   }

   {
      const std::string operators = "+-*/";
      std::size_t opr_index = 0;
      for (std::size_t i = 0; i < e_4.size(); ++i)
      {
         std::string curr_str = "(" + e_4[i] + operators[opr_index] + e_4[(i  + 10) % e_4.size()] + ")";
         e_4_comb.push_back(curr_str);
         opr_index = (opr_index + 1) % operators.size();
      }
   }

   {
      const std::string operators = "+-*/";
      std::size_t opr_index = 0;
      for (std::size_t i = 0; i < e_4.size(); ++i)
      {
         std::string curr_str = "(" + e_4[i] + operators[opr_index] + e_3[i % e_3.size()] + ")";
         e_43_comb.push_back(curr_str);
         opr_index = (opr_index + 1) % operators.size();
      }
   }

   {
      const std::string operators = "+-*/";
      std::size_t opr_index = 0;
      for (std::size_t i = 0; i < e_4.size(); ++i)
      {
         std::string curr_str = "(" + e_3[i % e_3.size()] + operators[opr_index] + e_4[i] + ")";
         e_34_comb.push_back(curr_str);
         opr_index = (opr_index + 1) % operators.size();
      }
   }

   {
      const std::string operators = "+-*/";
      std::size_t opr_index = 0;
      for (std::size_t i = 0; i < e_4.size(); ++i)
      {
         std::size_t opr_index0 = opr_index;
         std::size_t opr_index1 = opr_index = (opr_index + 1) % operators.size();
         opr_index = (opr_index + 1) % operators.size();
         std::string curr_str = "(" + e_4[i] + operators[opr_index0] + e_3[i % e_3.size()] + operators[opr_index1] + e_4[(i + 3) % e_4.size()] + ")";
         e_434_comb.push_back(curr_str);
      }
   }

   {
      const std::string operators = "+-*/";
      std::size_t opr_index = 0;
      for (std::size_t i = 0; i < e_4.size(); ++i)
      {
         std::size_t opr_index0 = opr_index;
         std::size_t opr_index1 = opr_index = (opr_index + 1) % operators.size();
         opr_index = (opr_index + 1) % operators.size();
         std::string curr_str = "(" + e_3[i % e_3.size()] + operators[opr_index0] + e_4[i] + operators[opr_index1] + e_3[(i + 3) % e_3.size()] + ")";
         e_343_comb.push_back(curr_str);
      }
   }

   std::copy(e_3.begin(),e_3.end(),std::ostream_iterator<std::string>(std::cout,"\n"));
   std::copy(e_4.begin(),e_4.end(),std::ostream_iterator<std::string>(std::cout,"\n"));

   std::copy(e_3_comb.begin(),e_3_comb.end(),std::ostream_iterator<std::string>(std::cout,"\n"));
   std::copy(e_4_comb.begin(),e_4_comb.end(),std::ostream_iterator<std::string>(std::cout,"\n"));

   std::copy(e_43_comb.begin(),e_43_comb.end(),std::ostream_iterator<std::string>(std::cout,"\n"));
   std::copy(e_34_comb.begin(),e_34_comb.end(),std::ostream_iterator<std::string>(std::cout,"\n"));


   std::copy(e_434_comb.begin(),e_434_comb.end(),std::ostream_iterator<std::string>(std::cout,"\n"));
   std::copy(e_343_comb.begin(),e_343_comb.end(),std::ostream_iterator<std::string>(std::cout,"\n"));

   return 0;
}

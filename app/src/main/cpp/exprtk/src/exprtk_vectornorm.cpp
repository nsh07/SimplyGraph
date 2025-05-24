/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk L-Norm Of A Vector                                  *
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
#include <vector>

#include "exprtk.hpp"


template <typename T>
struct norm : public exprtk::igeneric_function<T>
{
   typedef typename exprtk::igeneric_function<T> igfun_t;
   typedef typename igfun_t::parameter_list_t    parameter_list_t;
   typedef typename igfun_t::generic_type        generic_type;
   typedef typename generic_type::scalar_view    scalar_t;
   typedef typename generic_type::vector_view    vector_t;

   using exprtk::igeneric_function<T>::operator();

   norm()
   : exprtk::igeneric_function<T>("V|VTT|VT|VTTT")
     /*
        Overloads:
        0. V    - vector
        1. VTT  - vector, r0, r1
        2. VT   - vector, l-power
        3. VTTT - vector, l-power, r0, r1
     */
   {}

   inline T operator()(const std::size_t& ps_index, parameter_list_t parameters)
   {
      unsigned int l = 2;

      // Determine the l-power
      if (
           (2 == ps_index) ||
           (3 == ps_index)
         )
      {
         if (!scalar_t(parameters[1]).to_uint(l))
            return std::numeric_limits<T>::quiet_NaN();
      }

      const vector_t vec(parameters[0]);

      std::size_t r0 = 0;
      std::size_t r1 = vec.size() - 1;

      // Determine the range over the vector
      if (
           (1 == ps_index) ||
           (3 == ps_index)
         )
      {
         if (!scalar_t(parameters[(1 == ps_index) ? 1 : 2]).to_uint(r0))
            return std::numeric_limits<T>::quiet_NaN();

         if (!scalar_t(parameters[(1 == ps_index) ? 2 : 3]).to_uint(r1))
            return std::numeric_limits<T>::quiet_NaN();
      }

      switch (l)
      {
         #define norm_def(N) case  N : return norm_impl<N>(vec,r0,r1);
         norm_def( 1) norm_def( 2) norm_def( 3) norm_def( 4)
         norm_def( 5) norm_def( 6) norm_def( 7) norm_def( 8)
         norm_def( 9) norm_def(10) norm_def(11) norm_def(12)
         norm_def(13) norm_def(14) norm_def(15) norm_def(16)
         norm_def(17) norm_def(18) norm_def(19) norm_def(20)
         norm_def(21) norm_def(22) norm_def(23) norm_def(24)
         norm_def(25) norm_def(26) norm_def(27) norm_def(28)
         norm_def(29) norm_def(30) norm_def(31) norm_def(32)
         default : return norm_impl(vec,l,r0,r1);
      }
   }

   template <unsigned int Pow>
   inline T norm_impl(const vector_t& vector, const std::size_t r0, const std::size_t r1)
   {
      T sum = T(0);

      for (std::size_t i = r0; i <= r1; ++i)
      {
         sum += exprtk::details::numeric::fast_exp<T,Pow>::result(std::abs(vector[i]));
      }

      switch (Pow)
      {
         case 1  : return sum;
         case 2  : return exprtk::details::numeric::sqrt(sum);
         default : return exprtk::details::numeric::pow(sum, T(1) / Pow);
      }
   }

   inline T norm_impl(const vector_t& vector, unsigned int pow, const std::size_t r0, const std::size_t r1)
   {
      T sum = T(0);

      for (std::size_t i = r0; i <= r1; ++i)
      {
         sum += exprtk::details::numeric::pow(std::abs(vector[i]),T(pow));
      }

      return exprtk::details::numeric::pow(sum,T(1) / pow);
   }
};

template <typename T>
void norm_of_vector()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   T x[] = { T(1), T(2), T(3), T(4), T(5) };

   std::vector<T> y;

   y.push_back(T(5)); y.push_back(T(6));
   y.push_back(T(7)); y.push_back(T(8));
   y.push_back(T(9)); y.push_back(T(0));

   symbol_table_t symbol_table;
   symbol_table.add_vector("x", x);
   symbol_table.add_vector("y", y);

   norm<T> norm_;

   symbol_table.add_function("norm",norm_);

   const std::string vector_norm_program[] =
      {
         " norm(x)          ",
         " norm(x,1)        ",
         " norm(x,2)        ",
         " norm(x,3)        ",
         " norm(x,4)        ",
         " norm(x,5)        ",
         " norm(2x+1)       ",
         " norm(2x+1,1)     ",
         " norm(2x+1,2)     ",
         " norm(2x+1,3)     ",
         " norm(2x+1,4)     ",
         " norm(2x+1,5)     ",
         " norm(2x+y/3-4)   ",
         " norm(2x+y/3-4,1) ",
         " norm(2x+y/3-4,2) ",
         " norm(2x+y/3-4,3) ",
         " norm(2x+y/3-4,4) ",
         " norm(2x+y/3-4,5) "
      };

   const std::size_t vecnorm_program_size = sizeof(vector_norm_program) / sizeof(std::string);

   parser_t parser;

   for (std::size_t i = 0; i < vecnorm_program_size; ++i)
   {
      expression_t expression;
      expression.register_symbol_table(symbol_table);

      parser.compile(vector_norm_program[i], expression);

      printf("%s = %15.8f\n",
             vector_norm_program[i].c_str(),
             expression.value());
   }
}

int main()
{
   norm_of_vector<double>();
   return 0;
}

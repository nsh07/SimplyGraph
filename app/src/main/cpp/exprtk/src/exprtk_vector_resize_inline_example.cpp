/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Vector Resize Inline Within Expression Example             *
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
#include <vector>
#include <map>

#include "exprtk.hpp"


template <typename T>
class my_vv_size_handler_t final : public exprtk::igeneric_function<T>
{
public:

   typedef typename exprtk::igeneric_function<T> igfun_t;
   typedef typename igfun_t::parameter_list_t    parameter_list_t;
   typedef typename igfun_t::generic_type        generic_type;
   typedef typename generic_type::vector_view    vector_t;
   typedef typename generic_type::scalar_view    scalar_t;
   typedef exprtk::vector_view<T>*               vv_ptr_t;
   typedef std::map<void*,vv_ptr_t>              map_t;

   using exprtk::igeneric_function<T>::operator();

   my_vv_size_handler_t()
   : exprtk::igeneric_function<T>("VT")
   {}

   inline T operator()(parameter_list_t parameters) override
   {
      vector_t vector(parameters[0]);
      size_t   new_size = static_cast<std::size_t>(scalar_t(parameters[1])());
      void*    key      = static_cast<void*>(&vector[0]);

      typename map_t::iterator itr = vector_map_.find(key);

      if (itr == vector_map_.end())
      {
         printf("my_vv_size_handler - Error: failed to find vector_view for address: %p\n",key);
         return T(0);
      }

      exprtk::vector_view<T>& vv = *itr->second;

      if (vv.base_size() < new_size)
      {
         printf("my_vv_size_handler - Error: vector_view for address: %p new size of %d greater than base_size of %d\n",
                key,
                static_cast<unsigned int>(new_size),
                static_cast<unsigned int>(vv.base_size()));
         return T(0);
      }

      return vv.set_size(new_size) ? T(1) : T(0);
   }

   void register_vector_view(exprtk::vector_view<T>& vec_view)
   {
      vector_map_[vec_view.data()] = &vec_view;
   }

private:

   map_t vector_map_;
};

template <typename T>
void vector_inline_resize_example()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string inline_resize_expression =
      " var vec_original_size := v[];                                     "
      "                                                                   "
      " for (var i := 1; i <= 2 * vec_original_size; i += 1)              "
      " {                                                                 "
      "    if (resize(v,i) == true and v[] == i and sum(2v) == (i^2 + i)) "
      "    {                                                              "
      "       println('Success: set size: ', i ,'  v[] = ', v[]);         "
      "    }                                                              "
      "    else                                                           "
      "    {                                                              "
      "       println('Error: Failed to resize vector v to size: ', i);   "
      "    }                                                              "
      " };                                                                ";

   std::vector<T> v0 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
   std::vector<T> v1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

   exprtk::vector_view<T> vv = exprtk::make_vector_view(v0, v0.size());


   exprtk::rtl::io::package<T> io_package;

   my_vv_size_handler_t<T> vv_size_handler;
   vv_size_handler.register_vector_view(vv);

   symbol_table_t symbol_table;
   symbol_table.add_vector  ("v", vv);
   symbol_table.add_function("resize", vv_size_handler);
   symbol_table.add_package (io_package);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   if (!parser.compile(inline_resize_expression, expression))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             inline_resize_expression.c_str());
      return;
   }

   vv.rebase(v0.data());
   vv_size_handler.register_vector_view(vv);
   expression.value();

   vv.rebase(v1.data());
   vv_size_handler.register_vector_view(vv);
   expression.value();
}

int main()
{
   vector_inline_resize_example<double>();
   return 0;
}


/*
   Build:
   c++ -pedantic-errors -Wall -Wextra -Werror -O3 -DNDEBUG -o exprtk_vector_inline_resize_example exprtk_vector_inline_resize_example.cpp -L/usr/lib -lstdc++ -lm
*/

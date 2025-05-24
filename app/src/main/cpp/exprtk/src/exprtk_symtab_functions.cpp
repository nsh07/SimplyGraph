/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Symbol Table Function Names Example                 *
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
struct myfunc final : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   myfunc()
   : exprtk::ifunction<T>(2)
   { exprtk::disable_has_side_effects(*this); }

   inline T operator()(const T& v1, const T& v2) override
   {
      return T(1) + (v1 * v2) / T(3);
   }
};

template <typename T>
inline T myotherfunc(T v0, T v1, T v2)
{
   return std::abs(v0 - v1) * v2;
}

template <typename T>
void symbol_table_function_names()
{
   typedef exprtk::symbol_table<T> symbol_table_t;

   symbol_table_t symbol_table;

   myfunc<T> mf;

   symbol_table.add_function("f1",[](T v0) -> T { return v0;});
   symbol_table.add_function("f2",[](T v0, T v1) -> T{ return v0 / v1;});

   symbol_table.add_function("myfunc"   , mf         );
   symbol_table.add_function("otherfunc", myotherfunc);

   for (const auto& func : symbol_table.get_function_list())
   {
      printf("function: %s\n",func.c_str());
   }
}

int main()
{
   symbol_table_function_names<double>();
   return 0;
}

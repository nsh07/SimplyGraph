/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Import Packages Examples                            *
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
#include <map>
#include <string>

#include "exprtk.hpp"


template <typename T>
class import_packages : public exprtk::igeneric_function<T>
{
public:

   typedef typename exprtk::igeneric_function<T> igfun_t;
   typedef typename igfun_t::parameter_list_t    parameter_list_t;
   typedef typename igfun_t::generic_type        generic_type;
   typedef typename generic_type::scalar_view    scalar_t;
   typedef typename generic_type::string_view    string_t;
   typedef exprtk::symbol_table<T>               symbol_table_t;
   typedef std::function<bool(symbol_table_t&)>  import_func_t;
   typedef std::map<std::string,import_func_t>   package_map_t;

   using igfun_t::operator();

   import_packages(symbol_table_t& symbol_table)
   : igfun_t("S")
   , symbol_table_(symbol_table)
   {
      igfun_t::has_side_effects() = false;

      package_map_.emplace(
         "exprtk.rtl.io"     ,
         [&](symbol_table_t& symbtab) -> bool { return symbtab.add_package(io_package); });

      package_map_.emplace(
         "exprtk.rtl.io.file",
         [&](symbol_table_t& symbtab) -> bool { return symbtab.add_package(file_package); });

      package_map_.emplace(
         "exprtk.rtl.vecops" ,
         [&](symbol_table_t& symbtab) -> bool { return symbtab.add_package(vector_package); });
   }

   inline T operator()(parameter_list_t parameters) override
   {
      const string_t package(parameters[0]);

      const auto itr = package_map_.find(exprtk::to_str(package));

      if (package_map_.end() == itr)
      {
         return T(0);
      }

      return itr->second(symbol_table_) ? T(1) : T(0);
   }

private:

   symbol_table_t& symbol_table_;
   package_map_t   package_map_;
   exprtk::rtl::io::package<T>       io_package;
   exprtk::rtl::io::file::package<T> file_package;
   exprtk::rtl::vecops::package<T>   vector_package;
};

template <typename T>
void import_package_example()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   symbol_table_t symbol_table;

   import_packages<T> import_package(symbol_table);

   symbol_table.add_function("import",import_package);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   const std::string import_packages_program =
      " import('exprtk.rtl.io'     ); "
      " import('exprtk.rtl.io.file'); "
      " import('exprtk.rtl.vecops' ); "
      "                               "
      " var v[7] := [1:1];            "
      "                               "
      " println('v: ', v);            "
      "                               "
      " reverse(v);                   "
      "                               "
      " println('reversed v: ', v);   "
      "                               ";

   parser.compile(import_packages_program, expression);

   expression.value();
}

int main()
{
   import_package_example<double>();
   return 0;
}

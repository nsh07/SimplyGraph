/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Group Example (add, create, contains, size, get, erase)    *
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


#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <map>
#include <set>
#include <string>

#include "exprtk.hpp"


namespace groups_impl
{

template <typename T>
struct groups_collection
{
   bool add_group(const std::string& group_name, const std::set<T>& set)
   {
      const auto itr = groups.find(group_name);

      if (itr != groups.end())
      {
         return false;
      }

      groups[group_name] = set;
      return true;
   }

   bool contains(const std::string& group_name) const
   {
      return groups.count(group_name) > 0;
   }

   using set_t = std::set<T>;
   using ptr_t = set_t*;

   ptr_t get(const std::string group_name)
   {
      auto itr = groups.find(group_name);
      if (itr == groups.end())
      {
         return nullptr;
      }
      return &(itr->second);
   }

   std::map<std::string, std::set<T>> groups;
};

template <typename T>
class create final : public exprtk::igeneric_function<T>
{
public:

   using igfun_t          = typename exprtk::igeneric_function<T>;
   using parameter_list_t = typename igfun_t::parameter_list_t;
   using generic_type     = typename igfun_t::generic_type;
   using string_t         = typename generic_type::string_view;

   using strgrps_cllctn   = groups_collection<std::string>;

   using exprtk::igeneric_function<T>::operator();

   create(groups_collection<std::string>& groups)
   : exprtk::igeneric_function<T>("S")
   , groups_(groups)
   { exprtk::rtl::io::file::details::perform_check<T>(); }

   inline T operator() (parameter_list_t parameters) override
   {
      const std::string group_name = to_str(string_t(parameters[0]));

      if (
           !groups_.contains(group_name) &&
           !groups_.add_group(group_name, {})
         )
      {
         return T(0);
      }

      auto ptr = groups_.get(group_name);
      const std::size_t ptr_size = sizeof(ptr);
      T result = T(0);

      std::memcpy(reinterpret_cast<char*>(&result ),
                  reinterpret_cast<char*>(&ptr),
                  ptr_size);

      return result;
   }

   strgrps_cllctn groups_;
};

template <typename SetType, typename T>
SetType* make_set_handle(T v)
{
   const std::size_t ptr_size = sizeof(SetType*);
   SetType* ptr = reinterpret_cast<SetType*>(0);

   std::memcpy(reinterpret_cast<exprtk::details::char_ptr>(&ptr),
               reinterpret_cast<exprtk::details::char_ptr>(&v  ),
               ptr_size);

   return ptr;
}

template <typename T>
class add final : public exprtk::igeneric_function<T>
{
public:

   using igfun_t          = typename exprtk::igeneric_function<T>;
   using parameter_list_t = typename igfun_t::parameter_list_t;
   using generic_type     = typename igfun_t::generic_type;
   using scalar_t         = typename generic_type::scalar_view;
   using string_t         = typename generic_type::string_view;
   using strgrps_cllctn   = groups_collection<std::string>;

   using exprtk::igeneric_function<T>::operator();

   add(groups_collection<std::string>& groups)
   : exprtk::igeneric_function<T>("TS*")
   , groups_(groups)
   { exprtk::rtl::io::file::details::perform_check<T>(); }

   inline T operator() (parameter_list_t parameters) override
   {
      auto ptr = scalar_t(parameters[0])();
      assert(ptr != T(0));
      auto& set = *make_set_handle<strgrps_cllctn::set_t>(ptr);
      return process(set, parameters);
   }

private:

   inline T process(strgrps_cllctn::set_t& set, const parameter_list_t& parameters)
   {
      for (std::size_t i = 1; i < parameters.size(); ++i)
      {
         set.insert(to_str(string_t(parameters[i])));
      }

      return T(1);
   }

   strgrps_cllctn groups_;
};

template <typename T>
class contains final : public exprtk::igeneric_function<T>
{
public:

   using igfun_t          = typename exprtk::igeneric_function<T>;
   using parameter_list_t = typename igfun_t::parameter_list_t;
   using generic_type     = typename igfun_t::generic_type;
   using scalar_t         = typename generic_type::scalar_view;
   using string_t         = typename generic_type::string_view;
   using strgrps_cllctn   = groups_collection<std::string>;

   using exprtk::igeneric_function<T>::operator();

   contains(groups_collection<std::string>& groups)
   : exprtk::igeneric_function<T>("TS*")
   , groups_(groups)
   { exprtk::rtl::io::file::details::perform_check<T>(); }

   inline T operator() (parameter_list_t parameters) override
   {
      auto ptr = scalar_t(parameters[0])();
      assert(ptr != T(0));
      const auto& set = *make_set_handle<strgrps_cllctn::set_t>(ptr);
      return process(set, parameters);
   }

private:

   inline T process(const strgrps_cllctn::set_t& set, parameter_list_t& parameters) const
   {
      for (std::size_t i = 1; i < parameters.size(); ++i)
      {
         if (set.count(to_str(string_t(parameters[i]))))
         {
            return T(1);
         }
      }

      return T(0);
   }

   strgrps_cllctn groups_;
};

template <typename T>
class size final : public exprtk::igeneric_function<T>
{
public:

   using igfun_t          = typename exprtk::igeneric_function<T>;
   using parameter_list_t = typename igfun_t::parameter_list_t;
   using generic_type     = typename igfun_t::generic_type;
   using scalar_t         = typename generic_type::scalar_view;
   using string_t         = typename generic_type::string_view;
   using strgrps_cllctn   = groups_collection<std::string>;

   using exprtk::igeneric_function<T>::operator();

   size(groups_collection<std::string>& groups)
   : exprtk::igeneric_function<T>("T")
   , groups_(groups)
   { exprtk::rtl::io::file::details::perform_check<T>(); }

   inline T operator() (parameter_list_t parameters) override
   {
      auto ptr = scalar_t(parameters[0])();
      assert(ptr != T(0));
      const auto& set = *make_set_handle<strgrps_cllctn::set_t>(ptr);
      return T(set.size());
   }

private:

   strgrps_cllctn groups_;
};

template <typename T>
class get final : public exprtk::igeneric_function<T>
{
public:

   using igfun_t          = typename exprtk::igeneric_function<T>;
   using parameter_list_t = typename igfun_t::parameter_list_t;
   using generic_type     = typename igfun_t::generic_type;
   using scalar_t         = typename generic_type::scalar_view;
   using string_t         = typename generic_type::string_view;
   using strgrps_cllctn   = groups_collection<std::string>;

   using exprtk::igeneric_function<T>::operator();

   get(groups_collection<std::string>& groups)
   : igfun_t("TT",igfun_t::e_rtrn_string)
   , groups_(groups)
   { exprtk::rtl::io::file::details::perform_check<T>(); }

   inline T operator() (std::string& result, parameter_list_t parameters) override
   {
      const auto ptr = scalar_t(parameters[0])();
      const int index = static_cast<int>(scalar_t(parameters[1])());

      assert(ptr != T(0));

      auto& set = *make_set_handle<strgrps_cllctn::set_t>(ptr);

      if (
           (index >= 0) &&
           (static_cast<std::size_t>(index) >= set.size())
         )
      {
         return T(0);
      }

      auto itr = set.begin();
      std::advance(itr,index);
      result = *itr;
      return T(1);
   }

private:

   strgrps_cllctn groups_;
};

template <typename T>
class erase final : public exprtk::igeneric_function<T>
{
public:

   using igfun_t          = typename exprtk::igeneric_function<T>;
   using parameter_list_t = typename igfun_t::parameter_list_t;
   using generic_type     = typename igfun_t::generic_type;
   using scalar_t         = typename generic_type::scalar_view;
   using string_t         = typename generic_type::string_view;
   using strgrps_cllctn   = groups_collection<std::string>;

   using exprtk::igeneric_function<T>::operator();

   erase(groups_collection<std::string>& groups)
   : exprtk::igeneric_function<T>("TS*")
   , groups_(groups)
   { exprtk::rtl::io::file::details::perform_check<T>(); }

   inline T operator() (parameter_list_t parameters) override
   {
      auto ptr = scalar_t(parameters[0])();
      assert(ptr != T(0));
      auto& set = *make_set_handle<strgrps_cllctn::set_t>(ptr);
      return process(set,parameters);
   }

private:

   inline T process(strgrps_cllctn::set_t& set, parameter_list_t& parameters)
   {
      std::size_t count = 0;
      for (std::size_t i = 1; i < parameters.size(); ++i)
      {
         count += set.erase(to_str(string_t(parameters[i])));
      }

      return T(count);
   }

   strgrps_cllctn groups_;
};


} // namespace groups_impl

template <typename T>
void groups_example()
{
   using symbol_table_t = exprtk::symbol_table<T>;
   using expression_t   = exprtk::expression<T>;
   using parser_t       = exprtk::parser<T>;

   using strset_t = std::set<std::string>;

   strset_t girl_names;
   strset_t boy_names;

   girl_names.insert("jane");
   girl_names.insert("jill");

   boy_names.insert("jack");
   boy_names.insert("jim" );
   boy_names.insert("john");

   groups_impl::groups_collection<std::string> names_groups;

   names_groups.add_group("girls", girl_names);
   names_groups.add_group("boys",  boy_names );

   assert(names_groups.contains("girls"));
   assert(names_groups.contains("boys" ));

   groups_impl::create  <T> group_create  (names_groups);
   groups_impl::add     <T> group_add     (names_groups);
   groups_impl::contains<T> group_contains(names_groups);
   groups_impl::size    <T> group_size    (names_groups);
   groups_impl::get     <T> group_get     (names_groups);
   groups_impl::erase   <T> group_erase   (names_groups);

   exprtk::rtl::io::println<T> println;

   symbol_table_t symbol_table;
   expression_t   expression;
   parser_t       parser;

   symbol_table.add_function("create"  , group_create  );
   symbol_table.add_function("add"     , group_add     );
   symbol_table.add_function("contains", group_contains);
   symbol_table.add_function("size"    , group_size    );
   symbol_table.add_function("get"     , group_get     );
   symbol_table.add_function("erase"   , group_erase   );
   symbol_table.add_function("println" , println       );

   expression.register_symbol_table(symbol_table);

   const std::string program =
      " /* Load the groups from symbol table */         "
      " var girls := create('girls');                   "
      " var boys  := create('boys' );                   "
      "                                                 "
      " if (not(girls))                                 "
      " {                                               "
      "    println('failed to create girls set!');      "
      "    return [];                                   "
      " };                                              "
      "                                                 "
      " if (not(boys))                                  "
      " {                                               "
      "    println('failed to create boys set!');       "
      "    return [];                                   "
      " };                                              "
      "                                                 "
      " var name := 'john';                             "
      "                                                 "
      " if (contains(girls, name))                      "
      " {                                               "
      "    println(name, ' in girls set');              "
      " }                                               "
      " else                                            "
      " {                                               "
      "    println(name, ' NOT in girls set');          "
      " };                                              "
      "                                                 "
      " if (contains(boys, name))                       "
      " {                                               "
      "    println(name, ' in boys set');               "
      " }                                               "
      " else                                            "
      " {                                               "
      "    println(name, ' NOT in boys set');           "
      " };                                              "
      "                                                 "
      " /* Create a group from scratch */               "
      " var pets := create('pets');                     "
      "                                                 "
      " add(pets, 'buddy');                             "
      " add(pets, 'fluffy', 'snowy');                   "
      " add(pets, 'ziggy', 'monty', 'teddy');           "
      "                                                 "
      " if (contains(pets, name))                       "
      " {                                               "
      "    println(name, ' in pets set');               "
      " }                                               "
      " else                                            "
      " {                                               "
      "    println(name, ' NOT in pets set');           "
      " };                                              "
      "                                                 "
      " if (contains(pets, name, 'fluffy'))             "
      " {                                               "
      "    println(name, ' or fluffy in pets set');     "
      " }                                               "
      " else                                            "
      " {                                               "
      "    println(name, ' or fluffy NOT in pets set'); "
      " };                                              "
      "                                                 "
      " println('List of \\'pets\\':');                 "
      "                                                 "
      " for (var i := 0; i < size(pets); i += 1)        "
      " {                                               "
      "    println('[', i ,']: ', get(pets,i));         "
      " };                                              "
      "                                                 "
      " if (erase(pets, 'fluffy', 'teddy') != 2)        "
      " {                                               "
      "    println('Failed to erase fluffy and teddy'); "
      "    return [];                                   "
      " };                                              "
      "                                                 "
      " println('List of \\'pets\\':');                 "
      "                                                 "
      " for (var i := 0; i < size(pets); i += 1)        "
      " {                                               "
      "    println('[', i ,']: ', get(pets,i));         "
      " };                                              "
      "                                                 ";

   if (!parser.compile(program, expression))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             program.c_str());
      return;
   }

   expression.value();

   // Benchmark
   //{
   //   constexpr auto rounds = 100000;
   //
   //   const auto begin = std::chrono::system_clock::now();
   //   {
   //      for (std::size_t i = 0; i < rounds; ++i)
   //      {
   //         expression.value();
   //      }
   //   }
   //   const auto end = std::chrono::system_clock::now();
   //
   //   const auto total_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
   //
   //   printf("[*] total time: %5.4fsec rate: %8.4feval/sec\n",
   //          total_time_ms / 1000.0,
   //          (1000.0 * rounds) / total_time_ms);
   //}
}

int main()
{
   groups_example<double>();
   return 0;
}


/*
   Build:
   c++ -pedantic-errors -Wall -Wextra -Werror -std=c++17 -O3 -DNDEBUG -o exprtk_groups_examples exprtk_groups_examples.cpp -lm
*/
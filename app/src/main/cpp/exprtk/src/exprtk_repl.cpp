/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk REPL (Read Evaluate Print Loop) Interface           *
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
#include <cstdio>
#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>

#include "exprtk.hpp"


template <typename T>
struct putch : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   putch() : exprtk::ifunction<T>(1) {}

   inline T operator()(const T& v)
   {
      printf("%c",static_cast<int>(v));
      return T(0);
   }
};

template <typename T>
struct putint : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   putint() : exprtk::ifunction<T>(1) {}

   inline T operator()(const T& v)
   {
      printf("%d",static_cast<int>(v));
      return T(0);
   }
};

template <typename T>
struct rnd_01 : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   rnd_01() : exprtk::ifunction<T>(0)
   { ::srand(static_cast<unsigned int>(time(NULL))); }

   inline T operator()()
   {
      // Note: Do not use this in production
      // Result is in the interval [0,1)
      return T(::rand() / T(RAND_MAX + 1.0));
   }
};

template <typename T>
class expression_processor
{
public:

   typedef exprtk::symbol_table<T>           symbol_table_t;
   typedef exprtk::expression<T>             expression_t;
   typedef exprtk::parser<T>                 parser_t;
   typedef exprtk::parser_error::type        error_t;
   typedef exprtk::function_compositor<T>    compositor_t;
   typedef typename compositor_t::function   function_t;
   typedef typename parser_t::settings_store settings_store_t;
   typedef exprtk::lexer::parser_helper      prsrhlpr_t;

   typedef typename parser_t::dependent_entity_collector::symbol_t symbol_t;
   typedef std::vector<symbol_t> symbol_list_t;

   expression_processor()
   : persist_symbol_table_      (false)
   , symbol_dump_               (false)
   , assignment_dump_           (false)
   , display_total_time_        (false)
   , display_total_compile_time_(false)
   , enable_usr_                (false)
   , disable_local_vardef_      (false)
   , batch_runs_cnt_            (0    )
   , compositor_(function_symbol_table_)
   #ifdef exprtk_enable_repl_variables
   , s0_("abcdefghijk")
   , s1_("abcdefghijk0123456789")
   , s2_("012345678901234567890123456789")
   , v0_({ 1, 1, 1 })
   , v1_({ 2, 2, 2, 2, 2})
   , v2_({ 3, 3, 3, 3, 3, 3, 3})
   , v3_({ 4, 4, 4, 4, 4, 4, 4, 4})
   , vv_(exprtk::make_vector_view(v2_,v2_.size()))
   #endif
   {
      symbol_table_.add_constants();

      symbol_table_.add_function("putch"  , putch_  );
      symbol_table_.add_function("putint" , putint_ );
      symbol_table_.add_function("rnd_01" , rnd_01_ );
      symbol_table_.add_package (fileio_package_    );
      symbol_table_.add_package (vecops_package_    );
      symbol_table_.add_package (io_package_        );

      symbol_table_.add_function("poly01", poly01_);
      symbol_table_.add_function("poly02", poly02_);
      symbol_table_.add_function("poly03", poly03_);
      symbol_table_.add_function("poly04", poly04_);
      symbol_table_.add_function("poly05", poly05_);
      symbol_table_.add_function("poly06", poly06_);
      symbol_table_.add_function("poly07", poly07_);
      symbol_table_.add_function("poly08", poly08_);
      symbol_table_.add_function("poly09", poly09_);
      symbol_table_.add_function("poly10", poly10_);
      symbol_table_.add_function("poly11", poly11_);
      symbol_table_.add_function("poly12", poly12_);

      #ifdef exprtk_enable_repl_variables
      symbol_table_.add_stringvar("s0", s0_);
      symbol_table_.add_stringvar("s1", s1_);
      symbol_table_.add_stringvar("s2", s2_);
      symbol_table_.add_vector   ("v0", v0_);
      symbol_table_.add_vector   ("v1", v1_);
      symbol_table_.add_vector   ("v2", v2_);
      symbol_table_.add_vector   ("v3", v3_);

      vv_ = exprtk::make_vector_view(v2_,v2_.size());
      symbol_table_.add_vector   ("vv", vv_);
      #endif

      compositor_.add_auxiliary_symtab(symbol_table_);

      arith_opr_["+"] = settings_store_t::e_arith_add;
      arith_opr_["-"] = settings_store_t::e_arith_sub;
      arith_opr_["*"] = settings_store_t::e_arith_mul;
      arith_opr_["/"] = settings_store_t::e_arith_div;
      arith_opr_["%"] = settings_store_t::e_arith_mod;
      arith_opr_["^"] = settings_store_t::e_arith_pow;

      assign_opr_[":="] = settings_store_t::e_assign_assign;
      assign_opr_["+="] = settings_store_t::e_assign_addass;
      assign_opr_["-="] = settings_store_t::e_assign_subass;
      assign_opr_["*="] = settings_store_t::e_assign_mulass;
      assign_opr_["/="] = settings_store_t::e_assign_divass;
      assign_opr_["%="] = settings_store_t::e_assign_modass;

      inequality_opr_[ "<"] = settings_store_t::e_ineq_lt;
      inequality_opr_["<="] = settings_store_t::e_ineq_lte;
      inequality_opr_["=="] = settings_store_t::e_ineq_eq;
      inequality_opr_[ "="] = settings_store_t::e_ineq_equal;
      inequality_opr_["!="] = settings_store_t::e_ineq_ne;
      inequality_opr_["<>"] = settings_store_t::e_ineq_nequal;
      inequality_opr_[">="] = settings_store_t::e_ineq_gte;
      inequality_opr_[ ">"] = settings_store_t::e_ineq_gt;

      clear_functions();
   }

  ~expression_processor()
   {
      clear_functions();
   }

   bool& persist_symbol_table()
   {
      return persist_symbol_table_;
   }

   bool& symbol_dump()
   {
      return symbol_dump_;
   }

   bool& assignment_dump()
   {
      return assignment_dump_;
   }

   bool& display_total_time()
   {
      return display_total_time_;
   }

   bool& display_total_compile_time()
   {
      return display_total_compile_time_;
   }

   bool& enable_usr()
   {
      return enable_usr_;
   }

   bool& disable_local_vardef()
   {
      return disable_local_vardef_;
   }

   void setup_symbol_table()
   {
      if (!persist_symbol_table_)
      {
         symbol_table_.clear_variables();
         symbol_table_.add_constants  ();
         symbol_table_.add_constant ("e", exprtk::details::numeric::constant::e);
      }
   }

   void process(std::string program)
   {
      program = trim_whitespace(program);

      if (program.empty())
         return;

      setup_symbol_table();

      expression_t expression;
      expression.register_symbol_table(symbol_table_);
      expression.register_symbol_table(function_symbol_table_);

      exprtk::timer compile_timer;
      compile_timer.start();

      if (enable_usr_)
         parser_.enable_unknown_symbol_resolver();
      else
         parser_.disable_unknown_symbol_resolver();

      if (disable_local_vardef_)
         parser_.settings().disable_local_vardef();
      else
         parser_.settings().enable_local_vardef();

      parser_.dec().collect_variables() = symbol_dump_;
      parser_.dec().collect_functions() = symbol_dump_;

      parser_.dec().collect_assignments() = assignment_dump_;

      if (!parser_.compile(program,expression))
      {
         printf("Error: %s\tExpression:%c%s\n",
                parser_.error().c_str(),
                ((std::string::npos != program.find_first_of('\n')) ? '\n' : ' '),
                ((program.size() < 200) ? program.c_str() : "....."));

         for (std::size_t i = 0; i < parser_.error_count(); ++i)
         {
            error_t error = parser_.get_error(i);

            printf("Err No.: %02d  Pos: %02d  Type: [%14s] Msg: %s\n",
                   static_cast<unsigned int>(i),
                   static_cast<unsigned int>(error.token.position),
                   exprtk::parser_error::to_str(error.mode).c_str(),
                   error.diagnostic.c_str());

            if (
                 (0 == i) &&
                 exprtk::parser_error::update_error(error,program)
               )
            {
               printf("Error (line: %d column: %d)\n",
                      static_cast<unsigned int>(error.line_no),
                      static_cast<unsigned int>(error.column_no));

               printf("%s \n",error.error_line.c_str());
               printf("%s^\n",std::string(error.column_no,'~').c_str());
            }
         }

         return;
      }

      compile_timer.stop();

      if (display_total_compile_time_)
      {
         printf("\nCompile time: %6.3fms\n",compile_timer.time() * 1000.0);
      }

      if (batch_runs_cnt_)
      {
         std::vector<double> timings(batch_runs_cnt_,0.0);

         exprtk::timer total_timer;
         exprtk::timer timer;

         T result = T(0);

         total_timer.start();

         for (std::size_t i = 0; i < batch_runs_cnt_; ++i)
         {
            timer.start();

            result = expression.value();

            timer.stop();

            timings[i] = timer.time() * 1000.0;
         }

         total_timer.stop();

         printf("\nResult: %15.9f\n",result);

         std::sort(timings.begin(),timings.end());

         printf("\nRuns: %4d  Time min: %7.3fms   max: %7.3fms   avg: %7.3fms   tot: %7.3fms   90%%:%7.3fms\n",
                static_cast<unsigned int>(batch_runs_cnt_),
                timings.front(),
                timings.back (),
                std::accumulate(timings.begin(),timings.end(),0.0) / timings.size(),
                total_timer.time() * 1000.0,
                timings[static_cast<int>(timings.size() * 0.90)]);

         return;
      }

      exprtk::timer timer;
      timer.start();

      const T result = expression.value();

      timer.stop();

      if (expression.results().count())
      {
         print_results(expression.results());
      }

      printf("\nResult: %15.9f\n",result);

      if (display_total_time_)
      {
         printf("\nTotal time: %6.3fms\n",timer.time() * 1000.0);
      }

      if (symbol_dump_)
      {
         symbol_list_t symbol_list;

         parser_.dec().symbols(symbol_list);

         printf("------ Symbols ------\n");
         perform_symbol_dump(symbol_list);
         printf("---------------------\n");
      }

      if (assignment_dump_)
      {
         symbol_list_t assignment_list;

         parser_.dec().assignment_symbols(assignment_list);

         printf("---- Assignments ----\n");
         perform_symbol_dump(assignment_list);
         printf("---------------------\n");
      }
   }

   void process_from_file(const std::string& file_name)
   {
      if (file_name.empty())
         return;

      std::ifstream stream(file_name.c_str());

      if (!stream)
      {
         printf("ERROR: Failed to open file: %s\n\n",file_name.c_str());
         return;
      }

      std::string program(
                           (std::istreambuf_iterator<char>(stream)),
                           (std::istreambuf_iterator<char>())
                         );

      process_function_definition(program,false);
   }

   void process_directive(std::string expression)
   {
      expression = trim_whitespace(expression);

      if ('$' != expression[0])
         return;
      else if ("$enable_cache" == expression)
         persist_symbol_table() = true;
      else if ("$disable_cache" == expression)
         persist_symbol_table() = false;
      else if ("$enable_symbol_dump" == expression)
         symbol_dump() = true;
      else if ("$disable_symbol_dump" == expression)
         symbol_dump() = false;
      else if ("$enable_assignment_dump" == expression)
         assignment_dump() = true;
      else if ("$disable_assignment_dump" == expression)
         assignment_dump() = false;
      else if ("$enable_timer" == expression)
         display_total_time() = true;
      else if ("$enable_compile_timer" == expression)
         display_total_compile_time() = true;
      else if ("$disable_timer" == expression)
         display_total_time() = false;
      else if ("$disable_compile_timer" == expression)
         display_total_compile_time() = false;
      else if ("$enable_usr" == expression)
         enable_usr() = true;
      else if ("$disable_usr" == expression)
         enable_usr() = false;
      else if ("$enable_local_vardef" == expression)
         disable_local_vardef() = false;
      else if ("$disable_local_vardef" == expression)
         disable_local_vardef() = true;
      else if ("$list_vars" == expression)
         list_symbols();
      else if ("$clear_functions" == expression)
         clear_functions();
      else if ((0 == expression.find("$batch_run ")) && (expression.size() >= 12))
         process_batch_run(expression.substr(11,expression.size() - 11));
      else if ((0 == expression.find("$load ")) && (expression.size() > 7))
         process_from_file(expression.substr(6,expression.size() - 6));
      else if ((0 == expression.find("$disable arithmetic ")) && (expression.size() >= 21))
         process_disable_arithmetic(expression.substr(20,expression.size() - 20));
      else if ((0 == expression.find("$disable assignment ")) && (expression.size() >= 21))
         process_disable_assignment(expression.substr(20,expression.size() - 20));
      else if ((0 == expression.find("$disable inequality ")) && (expression.size() >= 21))
         process_disable_inequality(expression.substr(20,expression.size() - 20));
      else if ((0 == expression.find("$enable arithmetic ")) && (expression.size() >= 20))
         process_enable_arithmetic(expression.substr(19,expression.size() - 19));
      else if ((0 == expression.find("$enable assignment ")) && (expression.size() >= 20))
         process_enable_assignment(expression.substr(19,expression.size() - 19));
      else if ((0 == expression.find("$enable inequality ")) && (expression.size() >= 20))
         process_enable_inequality(expression.substr(19,expression.size() - 19));
      else if ("$begin" == expression)
         process_multiline();
      else if (0 == expression.find("$function"))
         process_function_definition(expression);
      else
         printf("\nERROR - Invalid directive: %s\n",expression.c_str());
   }

private:

   void print_results(const exprtk::results_context<T>& results)
   {
      typedef exprtk::results_context<T> results_context_t;
      typedef typename results_context_t::type_store_t type_t;
      typedef typename type_t::scalar_view scalar_t;
      typedef typename type_t::vector_view vector_t;
      typedef typename type_t::string_view string_t;

      typename exprtk::details::numeric::details::number_type<T>::type num_type;

      printf("%s\n",std::string(10,'-').c_str());
      printf("Return Results (#%d)\n",static_cast<int>(results.count()));

      for (std::size_t i = 0; i < results.count(); ++i)
      {
         printf("[%02d]  ",static_cast<int>(i));

         type_t t = results[i];

         switch (t.type)
         {
            case type_t::e_scalar : printf("Scalar\t");
                                    exprtk::rtl::io::details::print_type("%10.5f",scalar_t(t)(),num_type);
                                    break;

            case type_t::e_vector : {
                                       printf("Vector\t");
                                       vector_t vector(t);

                                       for (std::size_t x = 0; x < vector.size(); ++x)
                                       {
                                          exprtk::rtl::io::details::print_type("%10.5f",vector[x],num_type);

                                          if ((x + 1) < vector.size())
                                             printf(" ");
                                       }
                                    }
                                    break;

            case type_t::e_string : printf("String\t");
                                    printf("%s",to_str(string_t(t)).c_str());
                                    break;

            default               : continue;
         }

         printf("\n");
      }

      printf("%s\n",std::string(10,'-').c_str());
   }

   void perform_symbol_dump(const symbol_list_t& variable_list) const
   {
      for (std::size_t i = 0; i < variable_list.size(); ++i)
      {
         const symbol_t& symbol = variable_list[i];

         switch (symbol.second)
         {
            case parser_t::e_st_variable : printf("[%02d] Variable  %s\n",
                                                  static_cast<int>(i),symbol.first.c_str());
                                           break;

            case parser_t::e_st_vector   : printf("[%02d] Vector    %s\n",
                                                  static_cast<int>(i),symbol.first.c_str());
                                           break;

            case parser_t::e_st_string   : printf("[%02d] String    %s\n",
                                                  static_cast<int>(i),symbol.first.c_str());
                                           break;

            case parser_t::e_st_function : printf("[%02d] Function  %s\n",
                                                  static_cast<int>(i),symbol.first.c_str());
                                           break;

            case parser_t::e_st_local_variable
                                         : printf("[%02d] LocalVar  %s\n",
                                                  static_cast<int>(i),symbol.first.c_str());
                                           break;

            case parser_t::e_st_local_vector
                                         : printf("[%02d] LocalVec  %s\n",
                                                  static_cast<int>(i),symbol.first.c_str());
                                           break;

            case parser_t::e_st_local_string
                                         : printf("[%02d] LocalStr  %s\n",
                                                  static_cast<int>(i),symbol.first.c_str());
                                           break;

            default                      : break;
         }
      }
   }

   void process_batch_run(const std::string& batch_runs_cnt)
   {
      batch_runs_cnt_ = atoi(batch_runs_cnt.c_str());
   }

   void process_multiline()
   {
      std::string program;

      for ( ; ; )
      {
         std::string line;

         std::cout << ">> ";
         std::getline(std::cin,line);

         line = trim_whitespace(line);

         if (line.empty())
            continue;
         else if ("$end" == line)
            break;
         else
            program += (line + "\n");
      }

      process(program);
   }

   struct function_definition
   {
      std::string name;
      std::string body;
      std::vector<std::string> var_list;

      void clear()
      {
         name    .clear();
         body    .clear();
         var_list.clear();
      }
   };

   enum func_parse_result
   {
      e_parse_unknown = 0,
      e_parse_success = 1,
      e_parse_partial = 2,
      e_parse_lexfail = 4,
      e_parse_notfunc = 8
   };

   struct parse_function_definition_impl : public exprtk::lexer::parser_helper
   {
      func_parse_result process(std::string& func_def, function_definition& fd)
      {
         if (!init(func_def))
            return e_parse_lexfail;

         if (!token_is(token_t::e_symbol,"function"))
            return e_parse_notfunc;

         if (!token_is(token_t::e_symbol,prsrhlpr_t::e_hold))
            return e_parse_partial;

         fd.name = current_token().value;

         next_token();

         if (!token_is(token_t::e_lbracket))
            return e_parse_partial;

         if (!token_is(token_t::e_rbracket))
         {
            std::vector<std::string> var_list;

            for ( ; ; )
            {
               // (x,y,z,....w)
               if (!token_is(token_t::e_symbol,prsrhlpr_t::e_hold))
                  return e_parse_partial;

               var_list.push_back(current_token().value);

               next_token();

               if (token_is(token_t::e_rbracket))
                  break;

               if (!token_is(token_t::e_comma))
                  return e_parse_partial;
            }

            var_list.swap(fd.var_list);
         }

         const std::size_t body_begin = current_token().position;
               std::size_t body_end   = current_token().position;

         int bracket_stack = 0;

         if (!token_is(token_t::e_lcrlbracket,prsrhlpr_t::e_hold))
            return e_parse_partial;

         for ( ; ; )
         {
            body_end = current_token().position;

            if (token_is(token_t::e_lcrlbracket))
               bracket_stack++;
            else if (token_is(token_t::e_rcrlbracket))
            {
               if (0 == --bracket_stack)
                  break;
            }
            else
            {
               if (lexer().finished())
                  return e_parse_partial;

               next_token();
            }
         }

         const std::size_t size = body_end - body_begin + 1;

         fd.body = func_def.substr(body_begin,size);

         const std::size_t index = body_begin + size;

         if (index < func_def.size())
            func_def = func_def.substr(index,func_def.size() - index);
         else
            func_def = "";

         return e_parse_success;
      }
   };

   func_parse_result parse_function_definition(std::string& func_def, function_definition& cf)
   {
      parse_function_definition_impl parser;
      return parser.process(func_def,cf);
   }

   std::string read_from_stdin()
   {
      std::string input;

      for ( ; ; )
      {
         std::string line;

         std::cout << ">> ";
         std::getline(std::cin,line);

         if (line.empty())
            continue;
         else if ("$end" == line)
            break;
         else
            input += (line + "\n");
      }

      if (!input.empty())
         input.erase(input.end() - 1);

      return input;
   }

   void process_function_definition(const std::string& func_def_header, bool read_stdin = true)
   {
      std::string func_def = func_def_header;

      if (read_stdin)
      {
         func_def += read_from_stdin();

         if (!func_def.empty() && ('$' == func_def[0]))
            func_def.erase(func_def.begin());
      }

      do
      {
         function_definition fd;

         func_parse_result fp_result = parse_function_definition(func_def,fd);

         if (e_parse_success == fp_result)
         {
            std::string vars;

            for (std::size_t i = 0; i < fd.var_list.size(); ++i)
            {
               vars += fd.var_list[i] + ((i < fd.var_list.size() - 1) ? "," : "");
            }

            function_t f(fd.name);

            for (std::size_t i = 0; i < fd.var_list.size(); ++i)
            {
               f.var(fd.var_list[i]);
            }

            f.expression(fd.body);

            if (function_symbol_table_.get_function(fd.name))
            {
               function_symbol_table_.remove_function(fd.name);

               for (std::size_t i = 0; i < func_def_list_.size(); ++i)
               {
                  if (exprtk::details::imatch(fd.name, func_def_list_[i].name))
                  {
                     func_def_list_.erase(func_def_list_.begin() + i);

                     break;
                  }
               }
            }

            if (!compositor_.add(f,true))
            {
               function_symbol_table_.remove_function(fd.name);

               printf("Error - Failed to add function: %s\n",fd.name.c_str());

               return;
            }

            printf("Function[%02d]\n",static_cast<int>(func_def_list_.size()));
            printf("Name: %s      \n",fd.name.c_str()                        );
            printf("Vars: (%s)    \n",vars.c_str()                           );
            printf("------------------------------------------------------\n");

            func_def_list_.push_back(fd);
         }
         else if (e_parse_notfunc != fp_result)
         {
            printf("Error - Critical parsing error - partial parse occurred\n");
            return;
         }
         else
            break;
      }
      while (!func_def.empty());

      if (!func_def.empty())
      {
         process(func_def);
      }
   }

   void list_symbols()
   {
      std::deque<std::pair<std::string,T> > variable_list;
      symbol_table_.get_variable_list(variable_list);

      std::size_t max_varname_length = 0;

      for (std::size_t i = 0; i < variable_list.size(); ++i)
      {
         max_varname_length = std::max(max_varname_length,variable_list[i].first.size());
      }

      for (std::size_t i = 0; i < variable_list.size(); ++i)
      {
         int pad_length = 0;

         if (max_varname_length > variable_list[i].first.size())
         {
            pad_length = static_cast<int>(max_varname_length - variable_list[i].first.size());
         }

         printf("%02d %s%*.*s %25.10f\n",
                static_cast<unsigned int>(i),
                variable_list[i].first.c_str(),
                pad_length,
                pad_length,
                std::string(max_varname_length,' ').c_str(),
                variable_list[i].second);
      }
   }

   void clear_functions()
   {
      func_def_list_.clear();
      function_symbol_table_.clear();
   }

   std::string trim_whitespace(std::string s)
   {
      static const std::string whitespace(" \n\r\t\b\v\f");

      if (!s.empty())
      {
         s.erase(0,s.find_first_not_of(whitespace));

         if (!s.empty())
         {
            std::size_t index = s.find_last_not_of(whitespace);

            if (std::string::npos != index)
               s.erase(index + 1);
            else
               s.clear();
         }
      }

      return s;
   }

   void process_disable_arithmetic(const std::string& arithmetic)
   {
      typename std::map<std::string,typename settings_store_t::settings_arithmetic_opr>::iterator itr;

      if (arith_opr_.end() != (itr = arith_opr_.find(arithmetic)))
      {
         parser_.settings()
            .disable_arithmetic_operation(itr->second);
      }
   }

   void process_disable_assignment(const std::string& assignment)
   {
      typename std::map<std::string,typename settings_store_t::settings_assignment_opr>::iterator itr;

      if (assign_opr_.end() != (itr = assign_opr_.find(assignment)))
      {
         parser_.settings()
            .disable_assignment_operation(itr->second);
      }
   }

   void process_disable_inequality(const std::string& inequality)
   {
      typename std::map<std::string,typename settings_store_t::settings_inequality_opr>::iterator itr;

      if (inequality_opr_.end() != (itr = inequality_opr_.find(inequality)))
      {
         parser_.settings()
            .disable_inequality_operation(itr->second);
      }
   }

   void process_enable_arithmetic(const std::string& arithmetic)
   {
      typename std::map<std::string,typename settings_store_t::settings_arithmetic_opr>::iterator itr;

      if (arith_opr_.end() != (itr = arith_opr_.find(arithmetic)))
      {
         parser_.settings()
            .enable_arithmetic_operation(itr->second);
      }
   }

   void process_enable_assignment(const std::string& assignment)
   {
      typename std::map<std::string,typename settings_store_t::settings_assignment_opr>::iterator itr;

      if (assign_opr_.end() != (itr = assign_opr_.find(assignment)))
      {
         parser_.settings()
            .enable_assignment_operation(itr->second);
      }
   }

   void process_enable_inequality(const std::string& inequality)
   {
      typename std::map<std::string,typename settings_store_t::settings_inequality_opr>::iterator itr;

      if (inequality_opr_.end() != (itr = inequality_opr_.find(inequality)))
      {
         parser_.settings()
            .enable_inequality_operation(itr->second);
      }
   }


private:

   bool persist_symbol_table_;
   bool symbol_dump_;
   bool assignment_dump_;
   bool display_total_time_;
   bool display_total_compile_time_;
   bool enable_usr_;
   bool disable_local_vardef_;
   std::size_t batch_runs_cnt_;

   symbol_table_t symbol_table_;
   symbol_table_t function_symbol_table_;
   parser_t parser_;
   compositor_t compositor_;

   putch  <T> putch_;
   putint <T> putint_;
   rnd_01 <T> rnd_01_;
   exprtk::rtl::io::file::package<T> fileio_package_;
   exprtk::rtl::vecops::package<T>   vecops_package_;
   exprtk::rtl::io::package<T>       io_package_;

   exprtk::polynomial<T, 1> poly01_;
   exprtk::polynomial<T, 2> poly02_;
   exprtk::polynomial<T, 3> poly03_;
   exprtk::polynomial<T, 4> poly04_;
   exprtk::polynomial<T, 5> poly05_;
   exprtk::polynomial<T, 6> poly06_;
   exprtk::polynomial<T, 7> poly07_;
   exprtk::polynomial<T, 8> poly08_;
   exprtk::polynomial<T, 9> poly09_;
   exprtk::polynomial<T,10> poly10_;
   exprtk::polynomial<T,11> poly11_;
   exprtk::polynomial<T,12> poly12_;

   std::vector<function_definition> func_def_list_;

   std::map<std::string,typename settings_store_t::settings_arithmetic_opr> arith_opr_;
   std::map<std::string,typename settings_store_t::settings_assignment_opr> assign_opr_;
   std::map<std::string,typename settings_store_t::settings_inequality_opr> inequality_opr_;

   #ifdef exprtk_enable_repl_variables
   std::string    s0_;
   std::string    s1_;
   std::string    s2_;
   std::vector<T> v0_;
   std::vector<T> v1_;
   std::vector<T> v2_;
   std::vector<T> v3_;
   exprtk::vector_view<T> vv_;
   #endif
};

template <typename T>
void repl(int argc, char* argv[])
{
   expression_processor<T> processor;

   if (argc > 1)
   {
      for (int i = 1; i < argc; ++i)
      {
         processor.process_from_file(argv[i]);
      }
   }
   else
   {
      for ( ; ; )
      {
         std::string expression;

         std::cout << ">> ";
         std::getline(std::cin,expression);

         if (expression.empty())
            continue;
         else if ("exit" == expression)
            break;
         else if ("quit" == expression)
            break;
         else if ('$' == expression[0])
            processor.process_directive(expression);
         else
            processor.process(expression);
      }
   }
}

int main(int argc, char* argv[])
{
   repl<double>(argc,argv);
   return 0;
}


/*

REPL commands:

  $enable_cache/$disable_cache
  Enable or disable caching of variables.

  $enable_symbol_dump/$disable_symbol_dump
  Enable or disable dumping of symbols found in expression during
  compilation process.

  $enable_assignment_dump/$disable_assignment_dump
  Enable or disable dumping of symbols that undergo assignment in
  expression.

  $enable_timer/$disable_timer
  Enable or disable expression evaluation timer.

  $enable_compile_timer/$disable_compile_timer
  Enable or disable compilation timer

  $enable_usr/$disable_usr
  Enable or disable unknown symbol resolver.

  $list_vars
  List variables found in the global symbol table.

  $clear_functions
  Clear all functions found in the global function symbol table.

  $load <program file name>
  Load the file as a complete program and execute.

  $begin/$end
  Pre/post-ambles for multiline expressions.


Example REPL Instructions:
 Step 1.1 Enter: var x := 3; var y := 4; var z := x + y; println(z / 2);
 Step 1.2 Enter: var x := 3; var y := 4; var z := x - y; putint(z / 2);


 Step 2.Enter the following multi-line program:
 --- snip ---

$begin
var x := 3;
var y := 4;
var z := sin(x / pi) * cos(y * pi);
println(z / 2);
$end

 --- snip ---


 Step 3.1 Create a new file called mandelbrot.txt
 Step 3.2 Copy the contents between the snip lines into the file
 Step 3.3 Execute: ./exprtk_repl mandelbrot.txt


 Step 4.1 Execute: ./exprtk_repl
 Step 4.2 Enter: $load mandelbrot.txt

---- snip ----

var width    := 118;
var height   := 41;
var imag_max := +1;
var imag_min := -1;
var real_max := +1;
var real_min := -2.5;
var x_step   := (real_max - real_min) / width;
var y_step   := (imag_max - imag_min) / height;

for (var y := 0; y < height; y += 1)
{
   var imag := imag_min + (y_step * y);

   for (var x := 0; x < width; x += 1)
   {
      var real       := real_min + x_step * x;
      var z_real     := real;
      var z_imag     := imag;
      var plot_value := 0;

      for (var n := 0; n < 30; n += 1)
      {
         var a := z_real^2;
         var b := z_imag^2;

         plot_value := n;

         if ((a + b) < 4)
         {
            z_imag := 2 * z_real * z_imag + imag;
            z_real := a - b + real;
         }
         else
            break;
      };

      putch(61 - plot_value);

   };

   println()
}

---- snip ----


 Step 5.1 Copy into the REPL the contents of the snippet below:
---- snip ----
$function is_prime(x)
{
   if (x == 1)
      return [false];
   else if (x == 2)
      return [true];
   else if ((x % 2) == 0)
      return [false];

   var upper_bound := sqrt(x) + 1;

   for (var i := 3; i <= upper_bound; i += 2)
   {
      if (0 == (x % i))
      {
         return[false];
      }
   };

   return [true];
}
$end

$begin
for (var i := 1; i < 50; i += 1)
{
   if (is_prime(i))
   {
      println(i);
   }
}
$end

---- snip ----


 Step 6.1 Copy into the REPL the contents of the snippet below:
---- snip ----

$begin
var s := 'abcdefghijkl';
for (var i := 0; i < (s[] / 2); i+= 1)
{
   var j := s[] - i - 1;
   s[i:i + 1] <=> s[j:j + 1];
};

println(s)
$end

---- snip ----


 Step 7.1 Copy into the REPL the contents of the snippet below:
---- snip ----
$begin
var vec[11]    := { -1, -2, 3, 5, 6, -2, -1, 4, -4, 2, -1 };
var zero       := 0;
var max_sum    := 0;
var max_start  := 0;
var max_end    := 0;
var curr_sum   := 0;
var curr_start := 0;

for (var i := 0; i < vec[]; i += 1)
{
   curr_sum += vec[i];

   if (curr_sum < zero)
   {
      curr_sum   := 0;
      curr_start := i + 1;
   }
   else if (curr_sum > max_sum)
   {
      max_sum   := curr_sum;
      max_start := curr_start;
      max_end   := i;
   }
}

println('vec: ',vec);

println('Max sum:     ', max_sum  );
println('Start index: ', max_start);
println('End index:   ', max_end  );

for (var i := max_start; i <= max_end; i += 1)
{
   print(vec[i],' ');
}
$end

---- snip ----


 Step 8.1 Copy into the REPL the contents of the snippet below:
---- snip ----

$function is_prime(x)
{
   if (x <= 0)
      return [false];
   else if (frac(x) != 0)
      return [false];
   else
   {
      switch
      {
         case 1 == x : return [false];
         case 2 == x : return [true ];
         default     :
         {
            var prime_lut[160] :=
            {
                 2,   3,   5,   7,  11,  13,  17,  19,  23,  29,
                31,  37,  41,  43,  47,  53,  59,  61,  67,  71,
                73,  79,  83,  89,  97, 101, 103, 107, 109, 113,
               127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
               179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
               233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
               283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
               353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
               419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
               467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
               547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
               607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
               661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
               739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
               811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
               877, 881, 883, 887, 907, 911, 919, 929, 937, 941
            };

            var upper_bound := min(x - 1,trunc(sqrt(x)) + 1);

            for (var i := 0; i < prime_lut[]; i += 1)
            {
               if (prime_lut[i] >= upper_bound)
                  return [true];
               else if ((x % prime_lut[i]) == 0)
                  return [false];
            };

            var lower_bound := prime_lut[prime_lut[] - 1] + 2;

            for (var i := lower_bound; i < upper_bound; i += 2)
            {
               if ((x % i) == 0)
               {
                  return [false];
               }
            }
         };
      }
   };

   return [true];
}

var prime_count := 0;

for (var i := 1; i < 10^6; i += 1)
{
   if (is_prime(i))
   {
      prime_count += 1;
   }
};

prime_count
$end
---- snip ----


Step 9.1 Copy into the REPL the contents of the snippet below:
---- snip ----
$begin
var file := open('file.txt','w');
var s    := 'Hello world...\n';

for (var i := 0; i < 10; i += 1)
{
   write(file,s);
};

close(file);

println('~~~~~~~~~~~~~~~~~~~~~~');

file  := open('file.txt','r');
var i := 0;

while (not(eof(file)))
{
   s := getline(file);

   if (s[] > 0)
   {
      println('[',i+=1,'] - ',s);
   }
};

close(file);
$end
---- snip ----


Step 10.1 Copy into the REPL the contents of the snippet below:
---- snip ----
$begin
var v0[1000] := [rnd_01];
var v1[1000] := [0];

~{
 var file := open('data.dat','w');

 if (not(write(file,v0)))
 {
    println('failed to write vector 0\n');
    return [false];
 };

 close(file);
 };

~{
    var file := open('data.dat','r');

    if (not(read(file,v1)))
    {
       println('failed to read vector 1\n');
       return [false];
    };

    close(file);
 };

if (sum(v0 != v1) > 0)
   println('error: v0 != v1');
else
{
   println('success: v0 == v1');
   println('sum(v0) = ',sum(v0),'  avg(v0) = ',avg(v0));
   println('sum(v1) = ',sum(v1),'  avg(v1) = ',avg(v1));
}
$end
---- snip ----


Step 11.1 Copy into the REPL the contents of the snippet below:
---- snip ----
$function fibonacci(x)
{
   switch
   {
      case x == 0 : 0;
      case x == 1 : 1;
      default     :
      {
         var prev := 0;
         var curr := 1;
         while ((x -= 1) > 0)
         {
            curr += (curr <=> prev);
         };
      };
   }
}

for (var i := 0; i < 20; i += 1)
{
   println(i, ' = ', fibonacci(i));
};

$end
---- snip ----

*/

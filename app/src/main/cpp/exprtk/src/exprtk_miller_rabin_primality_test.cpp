/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk Miller-Rabin Probabilistic Primality Test           *
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
#include <array>
#include <cstdio>
#include <random>
#include <string>

#include "exprtk.hpp"


template <typename T>
struct random_uint final : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   random_uint()
   : exprtk::ifunction<T>(2)
   {
      std::random_device device;
      std::array<unsigned int,std::mt19937::state_size> seed;
      std::generate_n(seed.data(), seed.size(), std::ref(device));
      std::seed_seq seq(std::begin(seed), std::end(seed));
      generator.seed(seq);
   }

   inline T operator() (const T& r0,const T& r1) override
   {
      std::uniform_int_distribution<std::uint64_t>
         distribution { static_cast<std::uint64_t>(r0), static_cast<std::uint64_t>(r1) };

      return static_cast<T>(distribution(generator));
   }

   std::mt19937 generator;
};

template <typename T>
void miller_rabin_probabilistic_primality_test()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   T prime_numbers[] =
   {
      5083823,   5083879,   5083889,   5083909,   5083913,   5083927,   5083931,  5083957,
      5083973,   5083993,   5084017,   5084033,   5084069,   5084099,   5084111,  5084113,
      5084117,   5084129,   5084141,   5084147,   5084171,   5084179,   5084197,  5084213,
      5084221,   5084243,   5084249,   5084263,   5084269,   5084333,   5084371,  5084383,
      5084393,   5084399,   5084407,   5084423,   5084437,   5084447,   5084461,  5084473,
      5084477,   5084489,   5084501,   5084509,   5084531,   5084537,   5084543,  5084549,
      5084551,   5084557,   5084567,   5084593,   5084617,   5084627,   5084641,  5084671,
      5084689,   5084693,   5084711,   5084731,   5084743,   5084749,   5084753,  5084803,
      5084809,   5084813,   5084843,   5084851,   5084867,   5084869,   5084897,  5084903,
      6782807,   6782821,   6782827,   6782833,   6782869,   6782903,   6782911,  6782921,
      6782929,   6782933,   6782939,   6782959,   6782969,   6782977,   6783011,  6783031,
      6783047,   6783067,   6783083,   6783089,   6783097,   6783107,   6783131,  6783143,
      6783149,   6783151,   6783169,   6783193,   6783211,   6783223,   6783239,  6783241,
      6783253,   6783263,   6783299,   6783317,   6783331,   6783353,   6783421,  6783433,
      6783449,   6783457,   6783461,   6783467,   6783481,   6783521,   6783529,  6783533,
      9723667,   9723691,   9723697,   9723709,   9723713,   9723757,   9723761,  9723767,
      9723773,   9723797,   9723799,   9723803,   9723809,   9723821,   9723829,  9723841,
      9723851,   9723853,   9723869,   9723887,   9723893,   9723907,   9723913,  9723937,
      9723941,   9723943,   9723979,   9723997,   9724021,   9724061,   9724079,  9724087,
      9724097,   9724103,   9724129,   9724151,   9724171,   9724201,   9724207,  9724213,
      9724223,   9724241,   9724259,   9724277,   9724279,   9724283,   9724291,  9724327,
     10619129,  10619131,  10619137,  10619159,  10619177,  10619179,  10619209, 10619243,
     10619251,  10619263,  10619291,  10619303,  10619327,  10619341,  10619381, 10619393,
     10619431,  10619489,  10619501,  10619503,  10619513,  10619563,  10619573, 10619591,
     10619599,  10619617,  10619621,  10619641,  10619647,  10619669,  10619681, 10619711,
     10619723,  10619729,  10619731,  10619737,  10619759,  10619773,  10619783, 10619801,
     10619863,  10619893,  10619897,  10619899,  10619911,  10619923,  10619927, 10619957,
     10619971,  10619989,  10620007,  10620023,  10620041,  10620067,  10620131, 10620133,
     10620139,  10620143,  10620149,  10620151,  10620157,  10620163,  10620179, 10620199,
     10748671,  10748687,  10748693,  10748701,  10748707,  10748713,  10748747, 10748767,
     10748779,  10748783,  10748791,  10748807,  10748809,  10748827,  10748849, 10748863,
     10748867,  10748869,  10748873,  10748893,  10748897,  10748917,  10748921, 10748923,
     10748951,  10748953,  10748957,  10748963,  10748987,  10748999,  10749001, 10749023,
     10749029,  10749059,  10749061,  10749071,  10749077,  10749107,  10749121, 10749143,
     10749161,  10749163,  10749173,  10749191,  10749197,  10749199,  10749241, 10749251,
     10749259,  10749283,  10749313,  10749317,  10749337,  10749367,  10749371, 10749373,
     10749379,  10749391,  10749433,  10749437,  10749449,  10749461,  10749493, 10749581,
     10749587,  10749601,  10749617,  10749631,  10749659,  10749677,  10749679, 10749703,
     10749709,  10749719,  10749727,  10749743,  10749773,  10749779,  10749811, 10749821,
     10749833,  10749853,  10749857,  10749863,  10749881,  10749883,  10749889, 10749917,
     10749971,  10749983,  10749989,  10749997,  10750009,  10750021,  10750031, 10750037,
     10750049,  10750057,  10750073,  10750099,  10750127,  10750141,  10750147, 10750189,
     10750193,  10750249,  10750279,  10750303,  10750309,  10750319,  10750349, 10750373,
   };

   T composites[] =
   {
       199203677,  779234623,  843093203,  883543291, 1197162971,
      1282615157, 1552390397, 1765737859, 1878769589, 1993904873,
      2257133471, 2520523529, 2579094799, 2853450949, 2935025369,
      3095780533, 3164132249, 3408963511, 4260042859, 4608613981,
      4654875857, 5085931997, 7278175081, 7289187463, 9206112101
   };

   exprtk::rtl::io::println<T> println;
   random_uint<T>              random;

   symbol_table_t symbol_table;

   symbol_table.add_vector  ("prime_numbers"    , prime_numbers);
   symbol_table.add_vector  ("composite_numbers", composites   );
   symbol_table.add_function("println"          , println      );
   symbol_table.add_function("random"           , random       );

   compositor_t compositor(symbol_table);

   compositor.add(function_t()
      .name("modexp") // (base^exponent) mod m
      .var("b") // base
      .var("e") // exponent
      .var("m") // modulo
      .expression
      (
         " if (m == 1)                       "
         " {                                 "
         "    return [0];                    "
         " };                                "
         "                                   "
         " var result := 1;                  "
         " b %= m;                           "
         "                                   "
         " while (e > 0)                     "
         " {                                 "
         "    if ((e % 2) == 1)              "
         "    {                              "
         "       result := (result * b) % m; "
         "    };                             "
         "                                   "
         "    b := b^2 % m;                  "
         "    e := floor(e / 2);             "
         " };                                "
         "                                   "
         " result                            "
      ));

   compositor.add(function_t()
      .name("is_probable_prime")
      .var("n") // Number to test for primaility
      .var("k") // Number of iterations to perform
      .expression
      (
         " switch                                             "
         " {                                                  "
         "    case n <= 1       : return [false];             "
         "    case frac(n) != 0 : return [false];             "
         "    case n == 2       : return [true ];             "
         " };                                                 "
         "                                                    "
         " var primes[18] :=                                  "
         "     {                                              "
         "        2,  3,  5,  7, 11, 13, 17, 19, 23,          "
         "       29, 31, 37, 41, 43, 47, 53, 59, 61           "
         "     };                                             "
         "                                                    "
         " var upper_bound := min(n - 1, trunc(sqrt(n)) + 1); "
         "                                                    "
         " for (var i := 0; i < primes[]; i += 1)             "
         " {                                                  "
         "    if (primes[i] >= upper_bound)                   "
         "       return [true];                               "
         "    else if ((n % primes[i]) == 0)                  "
         "       return [false];                              "
         " };                                                 "
         "                                                    "
         " var s := n - 1;                                    "
         " var t := 0;                                        "
         "                                                    "
         " while ((s % 2) == 0)                               "
         " {                                                  "
         "    s := floor(s / 2);                              "
         "    t += 1;                                         "
         " };                                                 "
         "                                                    "
         " for (var trials := 0; trials < k; trials += 1)     "
         " {                                                  "
         "    var a := random(2, n - 2);                      "
         "    var x := modexp(a, s, n);                       "
         "                                                    "
         "    if ((x == 1) or (x == (n - 1)))                 "
         "    {                                               "
         "       continue;                                    "
         "    };                                              "
         "                                                    "
         "    for (var r := 1; r < t; r += 1)                 "
         "    {                                               "
         "       x := modexp(x, 2, n);                        "
         "                                                    "
         "       if (x == 1)                                  "
         "          return [false];                           "
         "       else if (x == (n - 1))                       "
         "          break;                                    "
         "    };                                              "
         "                                                    "
         "    if (x != (n - 1))                               "
         "    {                                               "
         "       return [false];                              "
         "    };                                              "
         " };                                                 "
         "                                                    "
         " return [true];                                     "
      ));

   const std::string miller_rabin_primality_test_program =
      "                                                     "
      " println('Testing prime numbers...');                "
      " for (var i := 0; i < prime_numbers[]; i += 1)       "
      " {                                                   "
      "    var n := prime_numbers[i];                       "
      "                                                     "
      "    if (is_probable_prime(n,1000))                   "
      "    {                                                "
      "       println(n, ' is correctly a prime number');   "
      "    }                                                "
      "    else                                             "
      "    {                                                "
      "       println(n, ' is NOT a prime number (error)'); "
      "    }                                                "
      " };                                                  "
      "                                                     "
      " println('Testing composite numbers...');            "
      " for (var i := 0; i < composite_numbers[]; i += 1)   "
      " {                                                   "
      "    var n := composite_numbers[i];                   "
      "                                                     "
      "    if (not(is_probable_prime(n,1000)))              "
      "    {                                                "
      "       println(n, ' is correctly a composite');      "
      "    }                                                "
      "    else                                             "
      "    {                                                "
      "       println(n, ' is a prime number (error)');     "
      "    }                                                "
      " };                                                  "
      "                                                     "
      " for (var i := 0; i < prime_numbers[]; i += 1)       "
      " {                                                   "
      "    for (var j := i; j < prime_numbers[]; j += 1)    "
      "    {                                                "
      "       var n := prime_numbers[i] * prime_numbers[j]; "
      "                                                     "
      "       if (not(is_probable_prime(n,1000)))           "
      "       {                                             "
      "          println(n, ' is correctly a composite');   "
      "       }                                             "
      "       else                                          "
      "       {                                             "
      "          println(n, ' is a prime number (error)');  "
      "       }                                             "
      "    }                                                "
      " };                                                  ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(miller_rabin_primality_test_program,expression);

   expression.value();
}

int main()
{
   miller_rabin_probabilistic_primality_test<double>();
   return 0;
}

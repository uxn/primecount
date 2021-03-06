///
/// @file  S2_easy.cpp
/// @brief Calculate the contribution of the clustered easy leaves
///        and the sparse easy leaves in parallel using OpenMP
///        (Deleglise-Rivat algorithm).
///
/// Copyright (C) 2015 Kim Walisch, <kim.walisch@gmail.com>
///
/// This file is distributed under the BSD License. See the COPYING
/// file in the top level directory.
///

#include <PiTable.hpp>
#include <primecount-internal.hpp>
#include <generate.hpp>
#include <int128.hpp>
#include <min_max.hpp>
#include <pmath.hpp>
#include <S2Status.hpp>
#include <fast_div.hpp>

#include <stdint.h>
#include <algorithm>
#include <vector>

#ifdef _OPENMP
  #include <omp.h>
#endif

using namespace std;
using namespace primecount;

namespace {
namespace S2_easy {

/// Calculate the contribution of the clustered easy leaves
/// and the sparse easy leaves.
/// @param T  either int64_t or uint128_t.
///
template <typename T1, typename T2>
T1 S2_easy(T1 x,
           int64_t y,
           int64_t z,
           int64_t c,
           vector<T2>& primes,
           int threads)
{
  T1 s2_easy = 0;
  int64_t x13 = iroot<3>(x);
  int64_t thread_threshold = 1000;
  threads = validate_threads(threads, x13, thread_threshold);

  PiTable pi(y);
  int64_t pi_sqrty = pi[isqrt(y)];
  int64_t pi_x13 = pi[x13];
  S2Status status(x);

  #pragma omp parallel for schedule(dynamic) num_threads(threads) reduction(+: s2_easy)
  for (int64_t b = max(c, pi_sqrty) + 1; b <= pi_x13; b++)
  {
    int64_t prime = primes[b];
    T1 x2 = x / prime;
    int64_t min_trivial_leaf = min(x2 / prime, y);
    int64_t min_clustered_easy_leaf = (int64_t) isqrt(x2);
    int64_t min_sparse_easy_leaf = z / prime;
    int64_t min_hard_leaf = max(y / prime, prime);

    min_sparse_easy_leaf = max(min_sparse_easy_leaf, min_hard_leaf);
    min_clustered_easy_leaf = max(min_clustered_easy_leaf, min_hard_leaf);
    int64_t l = pi[min_trivial_leaf];
    T1 sum = 0;

    // Find all clustered easy leaves:
    // n = primes[b] * primes[l]
    // x / n <= y && phi(x / n, b - 1) == phi(x / m, b - 1)
    // where phi(x / n, b - 1) = pi(x / n) - b + 2
    while (primes[l] > min_clustered_easy_leaf)
    {
      int64_t xn = (int64_t) fast_div(x2, primes[l]);
      int64_t phi_xn = pi[xn] - b + 2;
      int64_t last_prime = primes[b + phi_xn - 1];
      int64_t xm = max((int64_t) fast_div(x2, last_prime), min_clustered_easy_leaf);
      int64_t l2 = pi[xm];
      sum += phi_xn * (l - l2);
      l = l2;
    }

    // Find all sparse easy leaves:
    // n = primes[b] * primes[l]
    // x / n <= y && phi(x / n, b - 1) = pi(x / n) - b + 2
    for (; primes[l] > min_sparse_easy_leaf; l--)
    {
      int64_t xn = (int64_t) fast_div(x2, primes[l]);
      sum += pi[xn] - b + 2;
    }

    if (print_status())
      status.print(b, pi_x13);

    s2_easy += sum;
  }

  return s2_easy;
}

} // namespace S2_easy
} // namespace

namespace primecount {

int64_t S2_easy(int64_t x,
                int64_t y,
                int64_t z,
                int64_t c,
                int threads)
{
  print("");
  print("=== S2_easy(x, y) ===");
  print("Computation of the easy special leaves");
  print(x, y, c, threads);

  double time = get_wtime();
  vector<int32_t> primes = generate_primes(y);
  int64_t s2_easy = S2_easy::S2_easy((intfast64_t) x, y, z, c, primes, threads);

  print("S2_easy", s2_easy, time);
  return s2_easy;
}

#ifdef HAVE_INT128_T

int128_t S2_easy(int128_t x,
                 int64_t y,
                 int64_t z,
                 int64_t c,
                 int threads)
{
  print("");
  print("=== S2_easy(x, y) ===");
  print("Computation of the easy special leaves");
  print(x, y, c, threads);

  double time = get_wtime();
  int128_t s2_easy;

  // uses less memory
  if (y <= std::numeric_limits<uint32_t>::max())
  {
    vector<uint32_t> primes = generate_primes<uint32_t>(y);
    s2_easy = S2_easy::S2_easy((intfast128_t) x, y, z, c, primes, threads);
  }
  else
  {
    vector<int64_t> primes = generate_primes<int64_t>(y);
    s2_easy = S2_easy::S2_easy((intfast128_t) x, y, z, c, primes, threads);
  }

  print("S2_easy", s2_easy, time);
  return s2_easy;
}

#endif

} // namespace primecount

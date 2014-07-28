///
/// @file   ptypes.hpp
/// @brief  Convenience functions and typedefs for the non standard
///         __int128_t integer type.
///
/// Copyright (C) 2014 Kim Walisch, <kim.walisch@gmail.com>
///
/// This file is distributed under the BSD License. See the COPYING
/// file in the top level directory.
///

#ifndef PTYPES_HPP
#define PTYPES_HPP

#if defined(HAVE_INT128_T)

#include <stdint.h>

namespace primecount {

typedef int128_t maxint_t;

}

#elif defined(HAVE___INT128_T)

// int128_t is a typedef for __int128_t
#define HAVE_INT128_T

#include <ostream>
#include <string>

namespace primecount {

typedef __int128_t maxint_t;
typedef __int128_t int128_t;

inline std::ostream& operator<<(std::ostream& stream, int128_t n)
{
  std::string str;
  while (n > 0)
  {
    str += '0' + n % 10;
    n /= 10;
  }
  stream << std::string(str.rbegin(), str.rend());
  return stream;
}

} // namespace

#else /* int128_t not supported */

namespace primecount {

typedef int64_t maxint_t;

}

#endif

#endif /* PTYPES_HPP */
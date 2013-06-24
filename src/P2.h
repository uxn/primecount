#ifndef P2_PRIMECOUNT_H
#define P2_PRIMECOUNT_H

#include <primecount.h>
#include <stdint.h>

namespace primecount {

int64_t P2(int64_t x, int64_t a, int64_t b, int64_t pb, int threads = MAX_THREADS);

} // namespace primecount

#endif
#pragma once

#include <cstdint>

namespace dbi {

namespace util {

/// A random number generator, based on Prof. Thomas Neuman's algorithm
class Random {
public:
   Random(uint64_t seed = 88172645463325252ull);
   uint64_t rand();
   float randScaleFactor(); // returns float between 0 and 1
   uint64_t seed;
};

}

}

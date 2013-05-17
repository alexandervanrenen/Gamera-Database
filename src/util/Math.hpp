#pragma once

#include <cstdint>

namespace dbi {

namespace util {

// Credit to Warens Hacker's Delight
inline uint32_t nextPowerOfTwo(uint32_t n)
{
   n--;
   n |= n >> 1;
   n |= n >> 2;
   n |= n >> 4;
   n |= n >> 8;
   n |= n >> 16;
   return ++n;
}

// Credit to http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
// Read only code: just worship the gods that revealed it to mankind and don't attempt to understand it ;)
inline uint32_t countSetBits(uint32_t n)
{
   n = n - ((n >> 1) & 0x55555555);
   n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
   return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

}

}

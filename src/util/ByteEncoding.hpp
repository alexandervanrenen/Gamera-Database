#include "common/Config.hpp"
#include <array>
#include <cstdint>

namespace dbi {

namespace util {

inline uint16_t encodeBytes(uint16_t bytes)
{
   if(bytes<=1024) {  // use 8 bit to encode small values exp
      uint16_t ld=0;
      for(;bytes>0;bytes=bytes>>1)
         ld++;
      ld = ld>3 ? ld-3 : 0;
      return ld;
   } else { // use the remaining 8 bit to encode big values linear
      const uint16_t partitions = (kPageSize-1024) / 8;
      bytes-=1024;
      return 8+bytes/partitions;
   }
}

namespace {
constexpr std::array<uint16_t, 16> decodingTable = {{0, 8, 16, 32, 64, 128, 256, 512, 1024 // small exp
   , 1024 + ((kPageSize-1024) / 8) * 1 // big linear .. yeah, looks awesome, ey ? ;)
   , 1024 + ((kPageSize-1024) / 8) * 2
   , 1024 + ((kPageSize-1024) / 8) * 3
   , 1024 + ((kPageSize-1024) / 8) * 4
   , 1024 + ((kPageSize-1024) / 8) * 5
   , 1024 + ((kPageSize-1024) / 8) * 6
   , 1024 + ((kPageSize-1024) / 8) * 7}};
}

inline uint16_t decodeBytes(uint16_t code)
{
   return decodingTable[code];
}

}

}

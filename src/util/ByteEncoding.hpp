#include "common/Config.hpp"
#include <array>
#include <cstdint>

namespace dbi {

namespace util {

inline uint16_t encodeBytes(uint16_t bytes)
{
   if(bytes<=2048) {  // use 9 bit to encode small values exp
      uint16_t ld=0;
      for(;bytes>0;bytes=bytes>>1)
         ld++;
      ld = ld>3 ? ld-3 : 0;
      return ld;
   } else { // use the remaining 8 bit to encode big values linear
      const uint16_t partitions = (kPageSize-2048) / 7;
      bytes-=2048;
      return 9+bytes/partitions;
   }
}

namespace {
constexpr std::array<uint16_t, 16> decodingTable = {{0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 // small exp
   , 2048 + ((kPageSize-2048) / 7) * 1 // big linear .. yeah, looks awesome, ey ? ;)
   , 2048 + ((kPageSize-2048) / 7) * 2
   , 2048 + ((kPageSize-2048) / 7) * 3
   , 2048 + ((kPageSize-2048) / 7) * 4
   , 2048 + ((kPageSize-2048) / 7) * 5
   , 2048 + ((kPageSize-2048) / 7) * 6}};
}

inline uint16_t decodeBytes(uint16_t code)
{
   return decodingTable[code];
}

}

}

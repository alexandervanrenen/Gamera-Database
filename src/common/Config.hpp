#pragma once

#include <cstdint>

namespace dbi {

   using TID = uint32_t;
   using PageID = uint32_t;
   using SegmentID = uint32_t;

   static const uint32_t kPageSize = 1<<12;
}

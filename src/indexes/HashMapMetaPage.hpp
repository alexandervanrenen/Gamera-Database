#pragma once

#include "segment_manager/Extent.hpp"
#include "common/Config.hpp"
#include <cstdint>

namespace dbi {

struct HashMapMetaPage {
   // TODO think about required sizes
   uint64_t nextFreePageInternalPageId;
   uint8_t numRelevantBits;
   uint16_t directoryPageCount; // TODO: opt away
   uint32_t next;
   uint32_t size;
   uint64_t entries;
   PageId directoryPages;

   void addDirectoryPage(PageId id)
   {
      *((&directoryPages) + directoryPageCount) = id;
      directoryPageCount++;
   }

   PageId getDirectoryPage(uint32_t pos) const
   {
      assert(pos < directoryPageCount);
      return *((&directoryPages) + pos);
   }
};

}

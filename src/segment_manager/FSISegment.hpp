#pragma once

#include "Segment.hpp"
#include "common/Config.hpp"

namespace dbi {

class BufferManager;

class FSISegment : public Segment {
public:
   /// Constructor
   FSISegment(SegmentID id, std::vector<Extent> extents, BufferManager& bufferManager);

   /// Returns number of minimum free bytes
   uint32_t getFreeBytes(PageID id) const;
   /// Expects exact number of free bytes
   void setFreeBytes(PageID id, uint32_t freeBytes);
};

}

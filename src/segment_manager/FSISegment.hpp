#pragma once

#include "common/Config.hpp"
#include "Segment.hpp"

namespace dbi {

class BufferManager;

class FSISegment : public Segment {
public:
   /// Constructor
   FSISegment(SegmentId id, SegmentInventory& segmentInventory, BufferManager& bufferManager);
   virtual ~FSISegment();

   /// Returns number of minimum free bytes
   uint32_t getFreeBytes(PageId id) const;
   /// Expects exact number of free bytes
   void setFreeBytes(PageId id, uint32_t freeBytes);

   /// Get extents for this segment (extent is added by the segment inventory)
   virtual const Extent grow();
   virtual const Extent grow(uint64_t numPages);
};

}

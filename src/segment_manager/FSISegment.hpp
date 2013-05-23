#pragma once

#include "common/Config.hpp"
#include "Segment.hpp"

namespace dbi {

class BufferManager;

class FSISegment : public Segment {
public:
   /// Constructor
   FSISegment(SegmentId id, BufferManager& bufferManager, const std::vector<Extent>& extents);
   virtual ~FSISegment()
   {
   }

   // Add new extent to the segment (these pages need to be initialized for proper use)
   virtual void assignExtent(const Extent& extent);

   /// Returns number of minimum free bytes
   uint32_t getFreeBytes(PageId id) const;
   /// Expects exact number of free bytes
   void setFreeBytes(PageId id, uint32_t freeBytes);
};

}

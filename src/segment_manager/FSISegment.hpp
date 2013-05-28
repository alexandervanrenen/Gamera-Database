#pragma once

#include "common/Config.hpp"
#include "Segment.hpp"

namespace dbi {

class BufferManager;

class FSISegment : public Segment {
public:
   /// Constructor
   FSISegment(SegmentId id, BufferManager& bufferManager, const ExtentStore& extents);
   virtual ~FSISegment()
   {
   }

   /// Called by segment manager after a extent has been added to this object
   virtual void initializeExtent(const Extent& extent);

   /// Returns number of minimum free bytes
   uint32_t getFreeBytes(PageId id) const;
   /// Expects exact number of free bytes
   void setFreeBytes(PageId id, uint32_t freeBytes);
};

}

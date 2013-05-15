#pragma once

#include "Segment.hpp"
#include "common/Config.hpp"

namespace dbi {

class BufferManager;

class FSISegment : public Segment {
public:
   /// Constructor
   FSISegment(SegmentId id, BufferManager& bufferManager, const std::vector<Extent>& extents);
   virtual ~FSISegment() {}

   /// Returns number of minimum free bytes
   uint32_t getFreeBytes(PageId id) const;
   /// Expects exact number of free bytes
   void setFreeBytes(PageId id, uint32_t freeBytes);
};

}

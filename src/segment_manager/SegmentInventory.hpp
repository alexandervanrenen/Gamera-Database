#pragma once

#include "Extent.hpp"
#include "ExtentStore.hpp"
#include <unordered_map>

namespace dbi {

class SegmentInventory {
public:
   /// Constructor
   SegmentInventory(uint64_t numPages);

   /// Create an empty segment
   SegmentId createSegment();

   /// Add pages to a segment
   const Extent assignExtentToSegment(const SegmentId id, const uint32_t numPages);

   /// Access all extents of a given segment
   const ExtentStore& getExtentsOfSegment(const SegmentId id);

   /// Remove a segment and add its extents to free list
   void dropSegment(const SegmentId id);

private:
   /// Maps a segment id to all its extents
   std::unordered_map<SegmentId, ExtentStore> segmentMap;
   /// Stores free pages
   ExtentStore freePages;
   /// Keep track of min segment id
   SegmentId nextSegmentId;
};

}

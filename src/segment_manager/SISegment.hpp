#pragma once

#include "Extent.hpp"
#include <unordered_map>
#include <vector>

namespace dbi {

class SISegment {
public:
   /// Constructor
   SISegment(uint64_t numPages);

   /// Create an empty segment
   SegmentId createSegment();

   /// Add pages to a segment
   const Extent assignExtentToSegment(const SegmentId id, const uint32_t numPages);

   /// Access all extents of a given segment
   const std::vector<Extent> getExtentsOfSegment(const SegmentId id);

   /// Remove a segment and add its extents to free list
   void dropSegment(const SegmentId id);

private:
   /// Maps a segment id to all its extents
   std::unordered_map<SegmentId, std::vector<Extent>> segmentMap;
   /// Stores free pages
   std::vector<Extent> freePages;
   /// Keep track of min segment id
   SegmentId nextSegmentId;
};

}

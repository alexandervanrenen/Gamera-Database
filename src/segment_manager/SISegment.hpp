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
   SegmentID createSegment();

   /// Add pages to a segment
   const Extent assignExtentToSegment(const SegmentID id, const uint32_t numPages);

   /// Access all extents of a given segment
   const std::vector<Extent> getExtentsOfSegment(const SegmentID id);

   /// Remove a segment and add its extents to free list
   void dropSegment(const SegmentID id);

private:
   /// Maps a segment id to all its extents
   std::unordered_map<SegmentID, std::vector<Extent>> segmentMap;
   /// Stores free pages
   std::vector<Extent> freePages;
   /// Keep track of min segment id
   SegmentID nextSegmentId;
};

}

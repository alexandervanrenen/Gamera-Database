#pragma once

#include "Extent.hpp"
#include <unordered_map>
#include <vector>

namespace dbi {

class SegmentInventory {
public:
   SegmentInventory(uint64_t numPages);

   SegmentID createSegment();

   const Extent assignExtendToSegment(const SegmentID id, const uint32_t numPages);

   const std::vector<Extent> getExtentsOfSegment(const SegmentID id);

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

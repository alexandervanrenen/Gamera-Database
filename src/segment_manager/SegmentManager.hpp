#pragma once

#include "common/Config.hpp"
#include "SegmentType.hpp"
#include "SegmentInventory.hpp"
#include <memory>
#include <unordered_map>

namespace dbi {

class BufferManager;
class SPSegment;
class Segment;

class SegmentManager {
public:
   /// Constructor
   SegmentManager(BufferManager& bufferManager);

   /// Add a new segment with one extent of numPages to the segment inventory
   SegmentID createSegment(SegmentType segmentType, uint32_t numPages);

   /// Add numPages to the already existing segment with the given id
   void growSegment(Segment& id); // Let SegmentManager choose
   void growSegment(Segment& id, uint32_t numPages);

   /// Remove the segment with the given id from the segment inventory
   void dropSegment(Segment& id);

   // Access segment with given id
   SPSegment& getSPSegment(const SegmentID id);

private:
    BufferManager& bufferManager;

    SegmentInventory segmentInventory;

    std::unordered_map<SegmentID, std::unique_ptr<Segment>> segments;
};

}

#pragma once

#include "CompactExtentStore.hpp"
#include "Extent.hpp"
#include "ExtentStore.hpp"
#include "Persister.hpp"
#include <unordered_map>

namespace dbi {

class SegmentInventory {
public:
   /// Constructor
   SegmentInventory(BufferManager& bufferManager, bool isInitialSetup);

   /// Create an empty segment
   SegmentId createSegment();

   /// Add pages to a segment
   const Extent growSegment(const SegmentId id);
   const Extent growSegment(const SegmentId id, const uint32_t numPages);

   /// Access all extents of a given segment
   const ExtentStore& getExtentsOfSegment(const SegmentId id);

   /// Remove a segment and add its extents to free list
   void dropSegment(const SegmentId id);

private:
   /// Keep track of min segment id
   SegmentId nextSegmentId;

   /// Maps a segment id to all its extents
   std::unordered_map<SegmentId, std::pair<TupleId, ExtentStore>> segmentMap;
   /// Helps storing the segment mapping on disc
   Persister persister;

   /// Stores free pages
   CompactExtentStore freePages;
};

}

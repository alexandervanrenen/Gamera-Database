#include "SegmentManager.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "util/Utility.hpp"
#include "SPSegment.hpp"
#include <iostream>
#include <cassert>

using namespace std;

namespace dbi {
    
SegmentManager::SegmentManager(BufferManager& bufferManager)
: bufferManager(bufferManager)
, segmentInventory(bufferManager.getNumPages())
{
}

SegmentID SegmentManager::createSegment(SegmentType segmentType, uint32_t numPages)
{
   assert(segmentType == SegmentType::SP);

   SegmentID id = segmentInventory.createSegment();
   segmentInventory.assignExtendToSegment(id, numPages);
   return id;
}

void SegmentManager::growSegment(Segment& segment)
{
   // Do exponential grow
   segment.getNumPages();
}

void SegmentManager::growSegment(Segment& segment, uint32_t numPages)
{
   // Get extend and add to segment
   Extent extent = segmentInventory.assignExtendToSegment(segment.getId(), numPages);
   segment.addExtent(extent);
}

void SegmentManager::dropSegment(Segment& segment)
{
   segmentInventory.dropSegment(segment.getId());
   segments.erase(segment.getId());
}

SPSegment& SegmentManager::getSPSegment(const SegmentID id)
{
   // Look if segment is already created
   auto iter = segments.find(id);
   if(iter != segments.end())
      return reinterpret_cast<SPSegment&>(*iter->second);

   // Otherwise create it
   auto result = segments.insert(make_pair(id, unique_ptr<Segment>(new SPSegment(id, segmentInventory.getExtentsOfSegment(id)))));
   return reinterpret_cast<SPSegment&>(*result.first->second);
}

}

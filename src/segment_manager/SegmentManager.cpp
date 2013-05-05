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
   SegmentID id = segmentInventory.getNextSegmentId();
   segmentInventory.assignExtendToSegment(id, numPages);
   return id;
}

SPSegment& SegmentManager::getSPSegment(const SegmentID& id)
{
   // Look if segment is already created
   auto iter = segments.find(id);
   if(iter != segments.end())
      return reinterpret_cast<SPSegment&>(*iter->second);

   // Otherwise create it
   auto result = segments.insert(make_pair(id, dbiu::make_unique<SPSegment>(id, segmentInventory.getExtentsOfSegment(id))));
   return *result.first->second;
}

}

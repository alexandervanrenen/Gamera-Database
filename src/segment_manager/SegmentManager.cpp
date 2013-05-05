#include "SegmentManager.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "util/Utility.hpp"
#include "SPSegment.hpp"
#include <iostream>
#include <cassert>

using namespace std;

namespace dbi {

SegmentManager::SegmentManager(BufferManager& bufferManager, bool isInitialSetup)
: bufferManager(bufferManager)
, segmentInventory(bufferManager.getNumDiscPages())
{
   if(isInitialSetup) {
      // Build free space inventory
      uint64_t FSIPages = (bufferManager.getNumDiscPages() + 1) / 2; // Required bytes
      FSIPages = FSIPages / kPageSize + (FSIPages%kPageSize != 0); // Required pages

      SegmentID fsiID = segmentInventory.createSegment();
      assert(fsiID == 1);
      segmentInventory.assignExtentToSegment(fsiID, FSIPages);
   }

   // Load free space inventory
   SegmentID fsiID = 1;
   auto extents = segmentInventory.getExtentsOfSegment(fsiID);
   freeSpaceInventory = dbiu::make_unique<FSISegment>(fsiID, extents, bufferManager);  
}

SegmentID SegmentManager::createSegment(SegmentType segmentType, uint32_t numPages)
{
   assert(segmentType == SegmentType::SP);

   SegmentID id = segmentInventory.createSegment();
   segmentInventory.assignExtentToSegment(id, numPages);
   return id;
}

void SegmentManager::growSegment(Segment& segment)
{
   // Do exponential grow
   uint64_t numPages;
   if(segment.getNumPages() < 32)
      numPages = segment.getNumPages(); else // exp(2) when small
      numPages = segment.getNumPages()* 1.25f - segment.getNumPages(); // exp(1.25) otherwise
   growSegment(segment, numPages);
}

void SegmentManager::growSegment(Segment& segment, uint32_t numPages)
{
   // Get extent and add to segment
   Extent extent = segmentInventory.assignExtentToSegment(segment.getId(), numPages);
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
   auto segment = unique_ptr<Segment>(new SPSegment(id, segmentInventory.getExtentsOfSegment(id), *freeSpaceInventory, bufferManager));
   auto result = segments.insert(make_pair(id, move(segment)));
   return reinterpret_cast<SPSegment&>(*result.first->second);
}

}

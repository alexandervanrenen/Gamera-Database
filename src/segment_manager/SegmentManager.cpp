#include "buffer_manager/BufferManager.hpp"
#include "SegmentManager.hpp"
#include "SPSegment.hpp"
#include "BTreeSegment.hpp"
#include "FSISegment.hpp"
#include "util/Utility.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

SegmentManager::SegmentManager(BufferManager& bufferManager, bool isInitialSetup)
: bufferManager(bufferManager)
, segmentInventory(bufferManager.getNumDiscPages())
{
   if(isInitialSetup) {
      // Calculate required pages for free space inventory
      uint64_t FSIPages = (bufferManager.getNumDiscPages() + 1) / 2; // Required bytes
      FSIPages = FSIPages / kPageSize + (FSIPages % kPageSize != 0); // Required pages

      // Build free space inventory
      SegmentId fsiID = segmentInventory.createSegment();
      auto extent = segmentInventory.assignExtentToSegment(fsiID, FSIPages);
      freeSpaceInventory = util::make_unique<FSISegment>(fsiID, bufferManager, segmentInventory.getExtentsOfSegment(fsiID));
      freeSpaceInventory->initializeExtent(extent);
   } else {
      // Load free space inventory
      SegmentId fsiID = 1;
      auto& extents = segmentInventory.getExtentsOfSegment(fsiID);
      freeSpaceInventory = util::make_unique<FSISegment>(fsiID, bufferManager, extents);
   }

   assert(freeSpaceInventory->getId() == 1); // for now bitches =) .. move this to meta segment later
}

SegmentManager::~SegmentManager()
{
}

SegmentId SegmentManager::createSegment(SegmentType segmentType, uint32_t numPages)
{
   //assert(segmentType == SegmentType::SP);
   // Create segment
   SegmentId id = segmentInventory.createSegment();
   unique_ptr<Segment> segment;
   if (segmentType == SegmentType::SP) {
      segment = unique_ptr<Segment>(new SPSegment(id, *this, bufferManager, segmentInventory.getExtentsOfSegment(id)));
   } else if (segmentType == SegmentType::BT) {
      segment = unique_ptr<Segment>(new BTreeSegment(id, *this, bufferManager, segmentInventory.getExtentsOfSegment(id)));
   }
   growSegment(*segment, numPages);

   // Store segment and return id (?)
   segments.insert(make_pair(id, move(segment)));
   return id;
}

void SegmentManager::growSegment(Segment& segment)
{
   // Do exponential grow
   uint64_t numPages;
   if(segment.getNumPages() < 32)
      numPages = segment.getNumPages();
   else
      // exp(2) when small
      numPages = segment.getNumPages() * 1.25f - segment.getNumPages(); // exp(1.25) otherwise
   growSegment(segment, numPages);
}

void SegmentManager::growSegment(Segment& segment, uint32_t numPages)
{
   // Get extent and add to segment
   Extent extent = segmentInventory.assignExtentToSegment(segment.getId(), numPages);
   segment.initializeExtent(extent);
}

void SegmentManager::dropSegment(Segment& segment)
{
   segmentInventory.dropSegment(segment.getId());
   segments.erase(segment.getId());
}

SPSegment& SegmentManager::getSPSegment(const SegmentId id)
{
   // Look if segment is already created
   auto iter = segments.find(id);
   if(iter != segments.end())
      return reinterpret_cast<SPSegment&>(*iter->second);

   // Otherwise create it
   auto segment = unique_ptr<Segment>(new SPSegment(id, *this, bufferManager, segmentInventory.getExtentsOfSegment(id)));
   auto result = segments.insert(make_pair(id, move(segment)));
   return reinterpret_cast<SPSegment&>(*result.first->second);
}

BTreeSegment& SegmentManager::getBTreeSegment(const SegmentId id)
{
   // Look if segment is already created
   auto iter = segments.find(id);
   if(iter != segments.end())
      return reinterpret_cast<BTreeSegment&>(*iter->second);

   // Otherwise create it
   auto segment = unique_ptr<Segment>(new BTreeSegment(id, *this, bufferManager, segmentInventory.getExtentsOfSegment(id)));
   auto result = segments.insert(make_pair(id, move(segment)));
   return reinterpret_cast<BTreeSegment&>(*result.first->second);
}

FSISegment& SegmentManager::getFSISegment()
{
   return *freeSpaceInventory;
}

}

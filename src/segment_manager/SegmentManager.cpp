#include "BTreeSegment.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "FSISegment.hpp"
#include "SegmentInventory.hpp"
#include "SegmentManager.hpp"
#include "SPSegment.hpp"
#include "util/Utility.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

SegmentManager::SegmentManager(BufferManager& bufferManager, bool isInitialSetup)
: bufferManager(bufferManager)
{
   segmentInventory = util::make_unique<SegmentInventory>(bufferManager, isInitialSetup);

   if(isInitialSetup) {
      // Build free space inventory
      SegmentId fsiID = segmentInventory->createSegment();
      freeSpaceInventory = util::make_unique<FSISegment>(fsiID, *segmentInventory, bufferManager);
      // Create SPSegments for MetadataManager
      SegmentId spMetadataRelationsId = createSegment(SegmentType::SP, 1);
      assert(spMetadataRelationsId == kSchemaSegmentId);
   } else {
      // Load free space inventory
      SegmentId fsiID = kFreeSpaceInventoryId;
      freeSpaceInventory = util::make_unique<FSISegment>(fsiID, *segmentInventory, bufferManager);
   }

   assert(freeSpaceInventory->getId() == kFreeSpaceInventoryId); // for now bitches =) .. move this to meta segment later
}

SegmentManager::~SegmentManager()
{
}

SegmentId SegmentManager::createSegment(SegmentType segmentType, uint32_t numPages)
{
   // Create segment
   SegmentId id = segmentInventory->createSegment();
   unique_ptr<Segment> segment;
   if (segmentType == SegmentType::SP) {
      segment = unique_ptr<Segment>(new SPSegment(id, *freeSpaceInventory, *segmentInventory, bufferManager));
   } else if (segmentType == SegmentType::BT) {
      segment = unique_ptr<Segment>(new BTreeSegment(id, *segmentInventory, bufferManager));
   }
   growSegment(*segment, numPages);

   // Store segment and return id (?)
   segments.insert(make_pair(id, move(segment)));
   return id;
}

void SegmentManager::growSegment(Segment& segment)
{
   segment.grow();
}

void SegmentManager::growSegment(Segment& segment, uint32_t numPages)
{
   assert(numPages>0);
   segment.grow(numPages);
}

void SegmentManager::dropSegment(Segment& segment)
{
   segmentInventory->dropSegment(segment.getId());
   segments.erase(segment.getId());
}

SPSegment& SegmentManager::getSPSegment(const SegmentId id)
{
   // Look if segment is already created
   auto iter = segments.find(id);
   if(iter != segments.end())
      return reinterpret_cast<SPSegment&>(*iter->second);

   // Otherwise create it
   auto segment = unique_ptr<Segment>(new SPSegment(id, *freeSpaceInventory, *segmentInventory, bufferManager));
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
   auto segment = unique_ptr<Segment>(new BTreeSegment(id, *segmentInventory, bufferManager));
   auto result = segments.insert(make_pair(id, move(segment)));
   return reinterpret_cast<BTreeSegment&>(*result.first->second);
}

FSISegment& SegmentManager::getFSISegment()
{
   return *freeSpaceInventory;
}

}

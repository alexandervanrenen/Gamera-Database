#include "SegmentInventory.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

SegmentInventory::SegmentInventory(BufferManager& bufferManager, bool isInitialSetup)
: nextSegmentId(0)
, persister(bufferManager, freePages)
{
   if(isInitialSetup)
      persister.create(segmentMap); else
      persister.load(segmentMap);
}

SegmentId SegmentInventory::createSegment()
{
   SegmentId sid = ++nextSegmentId;
   TId tid = persister.insert(sid, ExtentStore());
   segmentMap.insert(make_pair(sid, make_pair(tid, ExtentStore())));
   return sid;
}

const Extent SegmentInventory::assignExtentToSegment(const SegmentId sid, const uint32_t numPages)
{
   assert(segmentMap.count(sid)==1 && numPages>0);

   // Find free extent -- using first-fit
   for(uint64_t i = 0; i < freePages.get().size(); i++)
      if(numPages <= freePages.get()[i].numPages()) {
         Extent freeExtent = Extent{freePages.get()[i].begin(), freePages.get()[i].begin() + numPages};
         freePages.remove(freeExtent);
         auto& segment = segmentMap[sid];
         segment.second.add(freeExtent);
         segment.first = persister.update(segment.first, sid, segment.second);
         return freeExtent;
      }

   // "controlled shutdown" if not found
   assert(false && "out of pages :(");
   throw;
}

void SegmentInventory::dropSegment(const SegmentId id)
{
   assert(segmentMap.count(id) == 1);

   // Find segments extents
   auto iter = segmentMap.find(id);
   auto& segment = iter->second;

   // Add back to free list
   for(auto& extent : segment.second.get())
      freePages.add(extent);

   // Remove from segment -> extent mapping
   persister.remove(segment.first);
   segmentMap.erase(iter);
}

const ExtentStore& SegmentInventory::getExtentsOfSegment(const SegmentId id)
{
   assert(segmentMap.count(id) == 1);
   return segmentMap[id].second;
}

}

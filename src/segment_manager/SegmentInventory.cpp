#include "SegmentInventory.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

SegmentInventory::SegmentInventory(const uint64_t numPages)
: nextSegmentId(0)
{
   assert(numPages > 0);
   freePages.add(Extent {1, PageId(numPages)});
}

SegmentId SegmentInventory::createSegment()
{
   SegmentId id = ++nextSegmentId;
   segmentMap.insert(make_pair(id, ExtentStore()));
   return id;
}

const Extent SegmentInventory::assignExtentToSegment(const SegmentId id, const uint32_t numPages)
{
   assert(segmentMap.count(id)==1 && numPages>0);

   // Find free extent -- using first-fit
   for(uint64_t i = 0; i < freePages.get().size(); i++)
      if(numPages <= freePages.get()[i].numPages()) {
         Extent freeExtent = Extent{freePages.get()[i].begin(), freePages.get()[i].begin() + numPages};
         freePages.remove(freeExtent);
         segmentMap[id].add(freeExtent);
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
   auto& extents = iter->second;

   // Add back to free list
   for(auto& extent : extents.get())
      freePages.add(extent);

   // Remove from segment -> extent mapping
   segmentMap.erase(iter);
}

const ExtentStore& SegmentInventory::getExtentsOfSegment(const SegmentId id)
{
   assert(segmentMap.count(id) == 1);
   return segmentMap[id];
}

}

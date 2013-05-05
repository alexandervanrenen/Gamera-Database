#include "SegmentInventory.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

SegmentInventory::SegmentInventory(const uint64_t numPages)
: nextSegmentId(0)
{
   assert(numPages > 0);
   freePages.emplace_back(Extent{1, PageID(numPages)});
}

SegmentID SegmentInventory::createSegment()
{
   SegmentID id = ++nextSegmentId;
   segmentMap.insert(make_pair(id, vector<Extent>()));
   return id;
}

const Extent SegmentInventory::assignExtendToSegment(const SegmentID id, const uint32_t numPages)
{
   assert(segmentMap.count(id) == 1);

   // Find free extent and assign -- using first-fit
   for(uint64_t i=0; i<freePages.size(); i++) {
      if(numPages <= freePages[i].numPages()) {
         // Add storage to segment with given id
         Extent extent{freePages[i].begin, freePages[i].begin+numPages};
         segmentMap[id].emplace_back(extent);

         // Remove storage from the fitting extent
         freePages[i].begin += numPages;
         if(freePages[i].numPages() == 0)
            freePages.erase(freePages.begin() + i);
         return extent;
      }
   }

   // Otherwise there is no free extend of this size
   assert(false && "out of pages :(");
   throw;
}

void SegmentInventory::dropSegment(const SegmentID id)
{
   assert(segmentMap.count(id) == 1);

   auto iter = segmentMap.find(id);
   auto& extents = iter->second;
   for(auto& extent : extents)
      freePages.push_back(extent);
   segmentMap.erase(iter);
}

const vector<Extent> SegmentInventory::getExtentsOfSegment(const SegmentID id)
{
   assert(segmentMap.count(id) == 1);
   return segmentMap[id];
}

}

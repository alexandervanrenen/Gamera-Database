#include "SISegment.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

namespace dbi {

SISegment::SISegment(const uint64_t numPages)
: nextSegmentId(0)
{
   assert(numPages > 0);
   freePages.emplace_back(Extent{1, PageID(numPages)});
}

SegmentID SISegment::createSegment()
{
   SegmentID id = ++nextSegmentId;
   segmentMap.insert(make_pair(id, vector<Extent>()));
   return id;
}

const Extent SISegment::assignExtentToSegment(const SegmentID id, const uint32_t numPages)
{
   assert(segmentMap.count(id) == 1);

   // Find free extent and assign -- using first-fit
   for(uint64_t i=0; i<freePages.size(); i++) {
      if(numPages <= freePages[i].numPages()) {
         // Add storage to segment with given id
         Extent extent{freePages[i].begin, freePages[i].begin+numPages};
         auto& extents = segmentMap[id];
         bool found = false;

         // Merge to extent list
         for(auto& iter : extents)
            if(extent.end == iter.begin || extent.begin == iter.end) {
               iter = Extent{std::min(extent.begin, iter.begin), std::max(extent.end, iter.end)};
               found = true;
               break;
            }
         if(!found) // If failed append
            extents.emplace_back(extent);

         // Remove storage from the fitting extent
         freePages[i].begin += numPages;
         if(freePages[i].numPages() == 0)
            freePages.erase(freePages.begin() + i);
         return extent;
      }
   }

   // Otherwise there is no free extent of this size
   assert(false && "out of pages :(");
   throw;
}

void SISegment::dropSegment(const SegmentID id)
{
   assert(segmentMap.count(id) == 1);

   // Remove segment
   auto iter = segmentMap.find(id);
   auto& extents = iter->second;

   // Sort merge the segments extents back into the free list
   sort(extents.begin(), extents.end(), [](const Extent& lhs, const Extent& rhs){return lhs.begin < rhs.begin;});
   uint64_t pos = 0;
   for(auto& extent : extents) {
      // Find position in free list
      while(pos < freePages.size() && extent.end < freePages[pos].begin)
         pos++;

      // Merge extents or insert
      if(pos < freePages.size() && (extent.begin == freePages[pos].end || extent.end == freePages[pos].begin))
         freePages[pos] = Extent{min(extent.begin, freePages[pos].begin), max(extent.end, freePages[pos].end)}; else
         freePages.insert(freePages.begin()+pos, extent);
   }
   segmentMap.erase(iter);
}

const vector<Extent> SISegment::getExtentsOfSegment(const SegmentID id)
{
   assert(segmentMap.count(id) == 1);
   return segmentMap[id];
}

}

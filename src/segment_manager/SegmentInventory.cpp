#include "SegmentInventory.hpp"

using namespace std;

namespace dbi {

SegmentInventory::SegmentInventory(const uint64_t numPages)
: nextSegmentId(0)
{
   freePages.emplace_back(Extent{1, PageID(numPages)});
}

SegmentID SegmentInventory::getNextSegmentId()
{
   return ++nextSegmentId;
}

const Extent SegmentInventory::assignExtendToSegment(const SegmentID id, const uint32_t numPages)
{
   // Find free extent and assign -- using first-fit
   for(uint64_t i=0; i<freePages.size(); i++) {
      if(freePages[i].numPages() <= numPages) {
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
   return Extent{0, 0};
}

const vector<Extent> SegmentInventory::getExtentsOfSegment(const SegmentID id)
{
   return segmentMap[id];
}

}

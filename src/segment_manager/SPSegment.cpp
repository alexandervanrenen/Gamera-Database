#include "SPSegment.hpp"
#include "Record.hpp"
#include <iostream>

using namespace std;

struct SlottedPage {
   uint64_t LSN; // for recovery
   uint8_t slotCount; // number of used slots
   uint8_t firstFreeSlot; // to speed up locating free slots
   uint8_t dataStart; // lower end of the data
   uint8_t freeSpace; // space that would be available restructuring
};

namespace dbi {

SPSegment::SPSegment(SegmentID id, FSISegment& freeSpaceInventory, BufferManager& bufferManager)
: Segment(id, bufferManager)
, freeSpaceInventory(freeSpaceInventory)
{
   for(auto iter=beginPageID(); iter!=endPageID(); iter++) {
      cout << "ctor " << *iter << endl;
   }
}

// void SPSegment::addExtent(const Extent& extent)
// {
//    Segment::addExtent(extent);
//    for(PageID iter=extent.begin; iter!=extent.end; iter++) {
//       cout << "add " << iter << endl;
//    }
// }

TID SPSegment::insert(const Record& record)
{
   cout << "inserting it" << endl;
   cout << record.data() << endl;

   return 0;
}

}

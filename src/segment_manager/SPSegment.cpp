#include "SPSegment.hpp"
#include "Record.hpp"
#include "SlottedPage.hpp"
#include "FSISegment.hpp"
#include "buffer_manager/BufferManager.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SPSegment::SPSegment(SegmentID id, FSISegment& freeSpaceInventory, BufferManager& bufferManager)
: Segment(id, bufferManager)
, freeSpaceInventory(freeSpaceInventory)
{
}

void SPSegment::assignExtent(const Extent& extent)
{
   Segment::assignExtent(extent);
   for(PageID iter=extent.begin; iter!=extent.end; iter++) {
      auto& frame = bufferManager.fixPage(iter, kExclusive);
      auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
      sp.initialize();
      freeSpaceInventory.setFreeBytes(iter, sp.getFreeBytes());
      bufferManager.unfixPage(frame, kDirty);
   }
}

TID SPSegment::insert(const Record& record)
{
   for(auto iter=beginPageID(); iter!=endPageID(); iter++) {
      if(freeSpaceInventory.getFreeBytes(*iter) >= record.size()) {
         auto& frame = bufferManager.fixPage(*iter, kExclusive);
         auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
         RecordID id = sp.insert(record);
         freeSpaceInventory.setFreeBytes(*iter, sp.getFreeBytes());
         bufferManager.unfixPage(frame, kDirty);
         return (*iter<<16) + id;
      }
   }

   cout << "no i am full !!!" << endl;
   throw;
}

Record SPSegment::lookup(TID id)
{
   assert(any_of(beginPageID(), endPageID(), [id](const PageID& pid){return pid==toPageID(id);}));
   auto& frame = bufferManager.fixPage(toPageID(id), kShared);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   Record result = sp.lookup(toRecordID(id));
   bufferManager.unfixPage(frame, kClean);
   return result;
}

}

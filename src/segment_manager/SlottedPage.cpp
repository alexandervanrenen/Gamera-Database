#include "SlottedPage.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

void SlottedPage::initialize() {
   assert(kPageSize < (1<<16));
   assert(sizeof(SlottedPage)==kPageSize);
   LSN = -1;
   slotCount = 0;
   firstFreeSlot = 0;
   dataBegin = data.size(); // data.data() + dataBegin == first byte of used data
   freeBytes = data.size();
}

RecordId SlottedPage::insert(const Record& record) {
   // Clean memory if fragmented
   if(dataBegin<sizeof(Slot)*slotCount || dataBegin-sizeof(Slot)*slotCount<record.size()) {
      cout << "i am fragmented" << endl;
      throw;
   }

   // Find slot to place data in
   Slot* slot;
   for(slot=slotBegin()+firstFreeSlot; slot!=slotEnd(); slot++)
      if(slot->offset==0 && slot->bytes==0)
         break;

   // Insert data into slot
   dataBegin -= record.size();
   slot->offset = dataBegin;
   slot->bytes = record.size();
   memcpy(data.data()+slot->offset, record.data(), slot->bytes);

   // Update data
   freeBytes -= record.size();
   freeBytes -= slotEnd()==slot?sizeof(Slot):0;
   slotCount = max(slotCount, static_cast<uint16_t>(1+slot-slotBegin()));
   firstFreeSlot = slot - slotBegin();

   return firstFreeSlot;
}

Record SlottedPage::lookup(RecordId id)
{
   Slot* result = slotBegin() + id;
   return Record(data.data() + result->offset, result->bytes);
}

}

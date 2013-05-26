#include "SlottedPage.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdint.h>

using namespace std;

namespace dbi {

void SlottedPage::initialize()
{
   assert(kPageSize < (1 << 16));
   assert(sizeof(SlottedPage) == kPageSize);
   LSN = -1;
   slotCount = 0;
   firstFreeSlot = 0;
   dataBegin = data.size(); // data.data() + dataBegin == first byte of used data
   freeBytes = data.size();
}

RecordId SlottedPage::insert(const Record& record)
{
   assert(getBytesFreeForRecord() >= record.size());
   assert(dataBegin >= sizeof(Slot) * slotCount);
   assert(record.size() > 0);
   uint32_t count = getAllRecords(0).size();

   // Try to reuse old slot including its space
   for(Slot* slot=slotBegin() + firstFreeSlot; slot!=slotEnd(); slot++)
      if(slot->bytes < 0 && abs(slot->bytes) >= record.size()) {
         freeBytes -= record.size();
         slot->bytes = record.size();
         memcpy(data.data() + slot->offset, record.data(), slot->bytes);
         assert(getAllRecords(0).size() == count+1);
         assert(dataBegin >= sizeof(Slot) * slotCount);
         return slot - slotBegin();
      }

   // Otherwise: We have to insert it into the free space: ensure there is enough
   if(dataBegin - sizeof(Slot) * slotCount < record.size() + sizeof(Slot))
      defragment();
   assert(dataBegin - sizeof(Slot) * slotCount >= record.size() + sizeof(Slot));

   // Now we know that the new record will fit into the free space: find a slot
   Slot* slot;
   for(slot = slotBegin() + firstFreeSlot; slot != slotEnd(); slot++)
      if(slot->offset == 0)
         break;

   // The slot variable points to the slot to be used
   dataBegin -= record.size();
   slot->offset = dataBegin;
   slot->bytes = record.size();
   memcpy(data.data()+slot->offset, record.data(), slot->bytes);
   freeBytes -= record.size();
   freeBytes -= slotEnd() == slot ? sizeof(Slot) : 0;
   slotCount = max(slotCount, static_cast<uint16_t>(1 + slot - slotBegin()));
   firstFreeSlot = slot - slotBegin(); // No free slot before this one

   assert(getAllRecords(0).size() == count+1);
   assert(dataBegin >= sizeof(Slot) * slotCount);
   return firstFreeSlot;
}

Record SlottedPage::lookup(RecordId id) const
{
   assert(toRecordId(id) < slotCount);   
   const Slot* result = slotBegin() + id;
   assert(result->offset != 0);
   assert(result->bytes > 0);
   return Record(data.data() + result->offset, result->bytes);
}

void SlottedPage::remove(RecordId rId)
{
   assert(dataBegin >= sizeof(Slot) * slotCount);
   assert(rId < slotCount);

   Slot* target = slotBegin() + rId;
   // Check if tId leads to valid slot
   assert(target->offset != 0);
   assert(target->bytes != 0);
   freeBytes += target->bytes;
   target->bytes = -target->bytes;   
   firstFreeSlot = min(firstFreeSlot, rId);

   assert(dataBegin >= sizeof(Slot) * slotCount);
}

void SlottedPage::update(RecordId recordId, const Record& newRecord)
{
   // Get and check everything .. *pretty paranoid*
   Slot* slot = slotBegin() + recordId;
   assert(dataBegin >= sizeof(Slot) * slotCount);
   assert(recordId < slotCount);
   assert(newRecord.size() > 0);
   assert(slot->offset != 0);   
   assert(slot->bytes > 0);
   assert(slot->bytes>=newRecord.size() || (uint16_t)(newRecord.size()-slot->bytes) <= getBytesFreeForRecord());

   // In place update -- Re-use old space if new data fit into it
   if(newRecord.size() <= (uint16_t) slot->bytes) {
      memcpy(data.data() + slot->offset, newRecord.data(), newRecord.size());
      freeBytes += slot->bytes - newRecord.size();
      slot->bytes = newRecord.size();
      return;
   }

   // Remove record (so that de-fragment can recycle it) (keep slot .. does not matter)
   freeBytes += slot->bytes;
   slot->offset = 0;
   slot->bytes = 0;

   // Ensure there is enough
   if(dataBegin - sizeof(Slot) * slotCount < newRecord.size())
      defragment();
   assert(dataBegin - sizeof(Slot) * slotCount >= newRecord.size());

   // Insert it
   freeBytes -= newRecord.size();
   dataBegin -= newRecord.size();
   slot->offset = dataBegin;
   slot->bytes = newRecord.size();
   memcpy(data.data()+slot->offset, newRecord.data(), slot->bytes);
}

vector<pair<TId, Record>> SlottedPage::getAllRecords(PageId thisPageId) const
{
   // Find all slots with data
   vector<pair<TId, Record>> result;
   result.reserve(slotCount);
   for(auto slot = slotBegin(); slot != slotEnd(); slot++)
      if(slot->offset != 0 && slot->bytes > 0)
         result.emplace_back(make_pair(toTID(thisPageId, slot-slotBegin()), Record(data.data() + slot->offset, slot->bytes)));
   return result;
}

/// Defragments the page. Any slot with a record length <= 0 (deleted/ not used) is set to its initial state during the process. NOT THREADSAFE!
void SlottedPage::defragment()
{
    vector<uint16_t> slotIndices(slotCount);
    std::iota(slotIndices.begin(), slotIndices.end(), 0);
    // Sort indices so that slot with greatest offset is first entry
    std::sort(slotIndices.begin(), slotIndices.end(), [this](uint16_t x, uint16_t y){ return (slotBegin() + x)->offset > (slotBegin() + y)->offset; });

    // Start at max index of data
    dataBegin = data.size();
    for(uint16_t slotIndex : slotIndices) {
        Slot* currentSlot = slotBegin() + slotIndex;
        if(currentSlot->bytes <= 0) {
            // Deleted/ not used records can be ignored; Only reset its slot
            currentSlot->offset = 0;
            currentSlot->bytes = 0;
            firstFreeSlot = min(firstFreeSlot, slotIndex);
        } else {
            assert(currentSlot->offset > 0);
            assert(currentSlot->bytes > 0);

            // Move the entry 
            uint16_t newPosition = dataBegin - currentSlot->bytes;
            std::memmove(data.data() + newPosition, data.data() + currentSlot->offset, currentSlot->bytes);
            currentSlot->offset = newPosition;

            // Advance (towards beginning) in index by length of currently processed record
            dataBegin -= currentSlot->bytes;
            assert(dataBegin >= sizeof(Slot) * slotCount);
        }
    }
}

uint16_t SlottedPage::getBytesFreeForRecord() const
{
  return freeBytes > sizeof(Slot) ? freeBytes - sizeof(Slot) : 0;
}

uint16_t SlottedPage::maximumRecordSize()
{
  return kPageSize - 16 - 4;
}

void SlottedPage::dump() const
{
  for(const Slot* slot=slotBegin(); slot!=slotEnd(); slot++)
    cout << "slot: (" << slot->offset << ", " << slot->bytes << ")" << endl;
  cout << "LSN: " << LSN << "  ";
  cout << "slotCount: " << slotCount << "  ";
  cout << "firstFreeSlot: " << firstFreeSlot << "  ";
  cout << "dataBegin: " << dataBegin << "  ";
  cout << "freeBytes: " << freeBytes << "  " << endl;
}

}

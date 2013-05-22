#include "SlottedPage.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>
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
   // Clean memory if fragmented
   if(dataBegin < sizeof(Slot) * slotCount || dataBegin - sizeof(Slot) * slotCount < record.size()) {
      cout << "i am fragmented" << endl;
      throw;
   }

   // Find slot to place data in
   Slot* slot;
   for(slot = slotBegin() + firstFreeSlot; slot != slotEnd(); slot++)
      if(slot->offset == 0 && slot->bytes == 0)
         break;

   // Insert data into slot
   dataBegin -= record.size();
   slot->offset = dataBegin;
   slot->bytes = record.size();
   memcpy(data.data() + slot->offset, record.data(), slot->bytes);

   // Update data
   freeBytes -= record.size();
   freeBytes -= slotEnd() == slot ? sizeof(Slot) : 0;
   slotCount = max(slotCount, static_cast<uint16_t>(1 + slot - slotBegin()));
   firstFreeSlot = slot - slotBegin();

   return firstFreeSlot;
}

Record SlottedPage::lookup(RecordId id) const
{
   assert(toRecordId(id) < slotCount);
   const Slot* result = slotBegin() + id;
   return Record(data.data() + result->offset, result->bytes);
}

bool SlottedPage::remove(RecordId rId)
{
   assert(rId < slotCount);
   Slot* target = slotBegin() + rId;
   // Check if tId leads to valid slot
   assert(target->offset != 0);
   assert(target->bytes != 0);
   freeBytes += target->bytes;
   target->offset = 0;
   target->bytes = 0;
   firstFreeSlot = min(firstFreeSlot, rId);
   return true;
}

bool SlottedPage::tryInPageUpdate(RecordId oldRecordId, const Record& newRecord)
{
   assert(oldRecordId < slotCount);
   // Check if told record is valid
   Slot* currentlyUsedSlot = slotBegin() + oldRecordId;
   assert(currentlyUsedSlot->offset != 0);
   assert(currentlyUsedSlot->bytes != 0);

   // In place update -- Re-use old record if new data fit into it
   if(newRecord.size() <= currentlyUsedSlot->bytes) {
      memcpy(data.data() + currentlyUsedSlot->offset, newRecord.data(), newRecord.size());
      freeBytes += currentlyUsedSlot->bytes - newRecord.size();
      currentlyUsedSlot->bytes = newRecord.size();
      return true;
   }

   // In page update
   if(newRecord.size() <= currentlyUsedSlot->bytes + freeBytes) {
      remove(oldRecordId);
      insert(newRecord);
      return true;
   } else
      return false;
}

vector<Record> SlottedPage::getAllRecords() const
{
   // Find all slots with data
   vector<Record> result;
   result.reserve(slotCount);
   for(auto slot = slotBegin(); slot != slotEnd(); slot++)
      if(slot->offset != 0 && slot->bytes != 0)
         result.emplace_back(Record(data.data() + slot->offset, slot->bytes));
   return result;
}

/// Defragments the page. Any slot with a record length <= 0 (deleted/ not used) is set to its initial state during the process. NOT THREADSAFE!
void SlottedPage::defragment(){
    
    vector<uint16_t> slotIndices;
    slotIndices.reserve(slotCount);
    std::iota(slotIndices.begin(), slotIndices.end(), 0);
    
    // Sort indices so that slot with greatest offset is first entry
    std::sort(slotIndices.begin(), slotIndices.end(), [this](uint16_t x, uint16_t y){ return (slotBegin() + x)->offset > (slotBegin() + y)->offset; });
    
    // Start at max index of data
    uint16_t currentOffset = data.size() - 1;
    Slot* currentSlot;
    for(auto slotIndex = slotIndices.begin(); slotIndex != slotIndices.end(); slotIndex++){
        currentSlot = slotBegin() + *slotIndex;
        
        // Deleted/ not used records can be ignored; Only reset its slot
        if(currentSlot->bytes <= 0){            
            currentSlot->offset = 0;
            currentSlot->bytes = 0;
        } else {
            uint16_t diffToBestPosition = currentOffset - currentSlot->offset - (currentSlot->bytes - 1);
            if(diffToBestPosition > 0 ){
                // Move the entry 
                std::memcpy(data.data() + currentSlot->offset + diffToBestPosition, data.data() + currentSlot->offset, currentSlot->bytes);
                currentSlot->offset += diffToBestPosition;
            }
            // Advance (towards beginning) in index by length of currently processed record
            currentOffset -= currentSlot->bytes;
        }
    }
}

}

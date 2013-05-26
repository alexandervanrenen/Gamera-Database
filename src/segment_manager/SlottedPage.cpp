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
   Slot* slot = aquireSlot(record.size());
   *slot = Slot(dataBegin, record.size(), Slot::Type::kNormal);
   memcpy(data.data()+slot->getOffset(), record.data(), slot->getLength());
   return slot - slotBegin();
}

Record SlottedPage::lookup(RecordId id) const
{
   assert(toRecordId(id) < slotCount);   
   const Slot* result = slotBegin() + id;
   assert(result->getOffset() != 0);
   assert(result->getLength() > 0);
   return Record(data.data() + result->getOffset(), result->getLength());
}

void SlottedPage::remove(RecordId rId)
{
   assert(dataBegin >= sizeof(Slot) * slotCount);
   assert(rId < slotCount);
   uint32_t count = getAllRecords(0).size();

   Slot* target = slotBegin() + rId;
   // Check if tId leads to valid slot
   assert(target->getOffset() != 0);
   assert(target->getLength() != 0);
   freeBytes += target->getLength();
   *target = Slot(target->getOffset(), target->getLength(), Slot::Type::kUnusedMemory);
   firstFreeSlot = min(firstFreeSlot, rId);

   assert(getAllRecords(0).size() == count - 1);
   assert(dataBegin >= sizeof(Slot) * slotCount);
}

void SlottedPage::update(RecordId recordId, const Record& newRecord)
{
   // Get and check everything .. *pretty paranoid*
   Slot* slot = slotBegin() + recordId;
   assert(dataBegin >= sizeof(Slot) * slotCount);
   assert(recordId < slotCount);
   assert(newRecord.size() > 0);
   assert(slot->getOffset() != 0);
   assert(slot->getLength() > 0);
   assert(slot->getLength()>=newRecord.size() || newRecord.size()-slot->getLength() <= getBytesFreeForRecord());

   // In place update -- Re-use old space if new data fit into it
   if(newRecord.size() <= slot->getLength()) {
      memcpy(data.data() + slot->getOffset(), newRecord.data(), newRecord.size());
      freeBytes += slot->getLength();
      freeBytes -= newRecord.size();
      slot->setLength(newRecord.size());
      return;
   }

   // Remove record (so that de-fragment can recycle it) (keep slot .. does not matter)
   freeBytes += slot->getLength();
   *slot = Slot(0, 0, Slot::Type::kNormal);

   // Ensure there is enough
   if(dataBegin - sizeof(Slot) * slotCount < newRecord.size())
      defragment();
   assert(dataBegin - sizeof(Slot) * slotCount >= newRecord.size());

   // Insert it
   freeBytes -= newRecord.size();
   dataBegin -= newRecord.size();
   *slot = Slot(dataBegin, newRecord.size(), Slot::Type::kNormal);
   memcpy(data.data()+slot->getOffset(), newRecord.data(), slot->getLength());
}

vector<pair<TId, Record>> SlottedPage::getAllRecords(PageId thisPageId) const
{
   // Find all slots with data
   vector<pair<TId, Record>> result;
   result.reserve(slotCount);
   for(auto slot = slotBegin(); slot != slotEnd(); slot++)
      if(slot->getOffset() != 0 && slot->isNormal())
         result.emplace_back(make_pair(toTID(thisPageId, slot-slotBegin()), Record(data.data() + slot->getOffset(), slot->getLength())));
   return result;
}

/// Defragments the page. Any slot with a record length <= 0 (deleted/ not used) is set to its initial state during the process. NOT THREADSAFE!
void SlottedPage::defragment()
{
    vector<uint16_t> slotIndices(slotCount);
    std::iota(slotIndices.begin(), slotIndices.end(), 0);
    // Sort indices so that slot with greatest offset is first entry
    std::sort(slotIndices.begin(), slotIndices.end(), [this](uint16_t x, uint16_t y){ return (slotBegin() + x)->getOffset() > (slotBegin() + y)->getOffset(); });

    // Start at max index of data
    dataBegin = data.size();
    for(uint16_t slotIndex : slotIndices) {
        Slot* currentSlot = slotBegin() + slotIndex;
        if(currentSlot->isMemoryUnused() || currentSlot->getOffset() == 0) {
            // Deleted/ not used records can be ignored; Only reset its slot
            *currentSlot = Slot(0, 0, Slot::Type::kNormal);
            firstFreeSlot = min(firstFreeSlot, slotIndex);
        } else {
            assert(currentSlot->getOffset() > 0);
            assert(currentSlot->getLength() > 0);

            // Move the entry 
            uint16_t newOffset = dataBegin - currentSlot->getLength();
            std::memmove(data.data() + newOffset, data.data() + currentSlot->getOffset(), currentSlot->getLength());
            currentSlot->setOffset(newOffset);

            // Advance (towards beginning) in index by length of currently processed record
            dataBegin -= currentSlot->getLength();
            assert(dataBegin >= sizeof(Slot) * slotCount);
        }
    }
}

uint16_t SlottedPage::getBytesFreeForRecord() const
{
  // Update first free slot
  for(;firstFreeSlot!=slotCount; firstFreeSlot++)
    if((slotBegin()+firstFreeSlot)->getOffset() == 0 || (slotBegin()+firstFreeSlot)->isMemoryUnused())
      break;

  if(firstFreeSlot != slotCount)
    return freeBytes; else
    return freeBytes > sizeof(Slot) ? freeBytes - sizeof(Slot) : 0;
}

uint16_t SlottedPage::maximumRecordSize()
{
  return kPageSize - 16 - 4;
}

void SlottedPage::dump() const
{
  for(const Slot* slot=slotBegin(); slot!=slotEnd(); slot++)
    cout << "slot: (" << slot->getOffset() << ", " << slot->getLength() << ") " << slot->isNormal() << " " << slot->isMemoryUnused() << " " << slot->isRedirectedFromOtherPage() << " " << slot->isRedirectedToOtherPage() << endl;
  cout << "LSN: " << LSN << "  ";
  cout << "slotCount: " << slotCount << "  ";
  cout << "firstFreeSlot: " << firstFreeSlot << "  ";
  cout << "dataBegin: " << dataBegin << "  ";
  cout << "freeBytes: " << freeBytes << "  " << endl;
}

bool SlottedPage::isValid() const
{
  uint32_t usedBytes = 0;
  for(const Slot* slot=slotBegin(); slot!=slotEnd(); slot++)
    if(slot->isNormal() && slot->getOffset() != 0)
      usedBytes += slot->getLength();
  return usedBytes + slotCount*sizeof(Slot) + freeBytes + 16 == kPageSize;
}

Slot* SlottedPage::aquireSlot(uint16_t length)
{
   assert(getBytesFreeForRecord() >= length);
   assert(dataBegin >= sizeof(Slot) * slotCount);
   assert(length > 0);
   uint32_t count = getAllRecords(0).size();

   // Try to reuse old slot including its space
   for(Slot* slot=slotBegin() + firstFreeSlot; slot!=slotEnd(); slot++)
      if(slot->isMemoryUnused() && slot->getLength() >= length) {
         freeBytes -= length;
         *slot = Slot(slot->getOffset() , length, Slot::Type::kNormal);
         assert(getAllRecords(0).size() == count+1);
         assert(dataBegin >= sizeof(Slot) * slotCount);
         return slot;
      }

   // Otherwise: We have to insert it into the free space: ensure there is enough
   if(dataBegin - sizeof(Slot) * slotCount < length + sizeof(Slot))
      defragment();
   assert(dataBegin - sizeof(Slot) * slotCount >= length);

   // Now we know that the new record will fit into the free space: find a slot
   Slot* slot;
   for(slot = slotBegin() + firstFreeSlot; slot != slotEnd(); slot++)
      if(slot->getOffset() == 0)
         break;

   // The slot variable points to the slot to be used
   dataBegin -= length;
   *slot = Slot(dataBegin, length, Slot::Type::kNormal);
   freeBytes -= length;
   freeBytes -= slotEnd() == slot ? sizeof(Slot) : 0;
   slotCount = max(slotCount, static_cast<uint16_t>(1 + slot - slotBegin()));
   firstFreeSlot = slot - slotBegin(); // No free slot before this one

   assert(getAllRecords(0).size() == count+1);
   assert(dataBegin >= sizeof(Slot) * slotCount);
   return slot;
}

uint32_t SlottedPage::countAllRecords() const
{
  throw;
}

const Slot* SlottedPage::slotBegin() const
{
  return reinterpret_cast<const Slot*>(data.data());
}

const Slot* SlottedPage::slotEnd() const
{
  return reinterpret_cast<const Slot*>(data.data()) + slotCount;
}

Slot* SlottedPage::slotBegin()
{
  return reinterpret_cast<Slot*>(data.data());
}

Slot* SlottedPage::slotEnd()
{
  return reinterpret_cast<Slot*>(data.data()) + slotCount;
}

}

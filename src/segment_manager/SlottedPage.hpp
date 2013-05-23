#pragma once

#include "common/Config.hpp"
#include "Record.hpp"
#include <cstdint>
#include <array>
#include <memory>

namespace dbi {

/// A page loaded by the BufferManager will be cast into a slotted page
class SlottedPage {
public:
   void initialize();

   RecordId insert(const Record& record);

   Record lookup(RecordId id) const;

   bool remove(RecordId rId);

   bool tryInPageUpdate(RecordId oldRecordId, const Record& newRecord);

   std::vector<std::pair<TId, Record>> getAllRecords(PageId thisPageId) const;

   /// Gets the number of bytes of this slotted page which have not been used yet. 
   /// This value is directly returned only if there is a slot which can be reused for a respective insert.
   /// Otherwise it is returned diminished by the size of a page slot to make sure a record as big as the return value definitely fits into the page.
   uint16_t getBytesFreeForRecord();   
   
   void defragment();
   
private:

   struct Slot {
      uint16_t offset;
      int16_t bytes;
   };

   uint64_t LSN; // for recovery
   uint16_t slotCount; // number of used slots
   uint16_t firstFreeSlot; // to speed up locating free slots
   uint16_t dataBegin; // lower end of the data
   uint16_t freeBytes; // space that would be available restructuring .. yeah ain't gonna happen ?

   std::array<char, kPageSize - 16> data; // 16 == size of header of this page

   const Slot* slotBegin() const
   {
      return reinterpret_cast<const Slot*>(data.data());
   }

   const Slot* slotEnd() const
   {
      return reinterpret_cast<const Slot*>(data.data()) + slotCount;
   }

   Slot* slotBegin()
   {
      return reinterpret_cast<Slot*>(data.data());
   }

   Slot* slotEnd()
   {
      return reinterpret_cast<Slot*>(data.data()) + slotCount;
   }

   SlottedPage() = delete; // Just in case you try :p
};

}

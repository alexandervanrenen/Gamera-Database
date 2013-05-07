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

   Record lookup(RecordId id);

   uint16_t getFreeBytes() {return freeBytes;}

private:

   struct Slot {
      uint16_t offset;
      uint16_t bytes;
   };

   uint64_t LSN; // for recovery
   uint16_t slotCount; // number of used slots
   uint16_t firstFreeSlot; // to speed up locating free slots
   uint16_t dataBegin; // lower end of the data
   uint16_t freeBytes; // space that would be available restructuring .. yeah ain't gonna happen ?

   std::array<char, kPageSize-16> data;
   Slot* slotBegin() {return reinterpret_cast<Slot*>(data.data());}
   Slot* slotEnd() {return reinterpret_cast<Slot*>(data.data()) + slotCount;}

   SlottedPage() = delete; // Just in case you try :p
};

}

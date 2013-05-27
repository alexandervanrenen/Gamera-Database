#pragma once

#include "common/Config.hpp"
#include "Record.hpp"
#include "Slot.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

/// A page loaded by the BufferManager will be cast into a slotted page
/// The slotted pages then offers some basic data management methods
class SlottedPage {
public:
   /// Called when page is first used to set up header
   void initialize();

   /// Add a record -- assumes canHoldRecord true
   RecordId insert(const Record& record);

   /// Store a record from another page here
   RecordId insertForeigner(const Record& record, TId originalPosition);

   /// Lookup a record -- The TID which is added to each foreign record is stripped away.
   Record lookup(RecordId id) const;

   /// Determines whether the record is really on this page -- kInvailTupleId if so otherwise the referenced page
   TId isReference(RecordId rid) const;

   /// Update record -- assumes canUpdateRecord
   void update(RecordId rid, const Record& newRecord);

   /// Update record -- assumes canUpdateForeignRecord
   void updateForeigner(RecordId rid, TId remoteTId, const Record& newRecord); // TODO: remove rid

   /// Use this if record could not be updated in-page
   void updateToReference(RecordId rid, TId newLocation);

   /// Remove the given rid
   void remove(RecordId rid);

   /// Check if the record can be placed on this page
   bool canHoldRecord(const Record& rid) const;
   bool canHoldForeignRecord(const Record& rid) const;
   bool canUpdateRecord(RecordId rid, const Record& newRecord) const;
   bool canUpdateForeignRecord(RecordId rid, const Record& newRecord) const;

   /// Get all Records actually on this pages (will ignore reference records and use foreign records)
   std::vector<std::pair<TId, Record>> getAllRecords(PageId thisPageId) const;

   /// Gets the number of bytes of this slotted page which have not been used yet.
   /// This value is directly returned only if there is a slot which can be reused for a respective insert.
   /// Otherwise it is returned diminished by the size of a page slot to make sure a record as big as the return value definitely fits into the page.
   uint16_t getBytesFreeForRecord() const;
   static uint16_t maximumRecordSize();

   /// Counts all records (use references and foreign records) .. used for debugging
   uint32_t countAllRecords() const;

   /// Close all gaps
   void defragment();

   /// Dump to page to standard output
   void dump() const;

   /// Do some counting to determine basic inconsistencies
   bool isValid() const;

private:
   /// Header
   uint64_t LSN; // for recovery
   uint16_t slotCount; // number of used slots
   mutable uint16_t firstFreeSlot; // ensures only that there is no other free slot in front of it
   uint16_t dataBegin; // lower end of the data
   uint16_t freeBytes; // space that would be available restructuring .. yeah ain't gonna happen ?

   /// Data
   std::array<char, kPageSize - 16> data; // 16 == size of header of this page

   /// Get a slot for the specified length and update the header. Caller only has to place data into the slot.
   Slot* prepareSlotForInsert(uint16_t length);
   Slot* prepareSlotForUpdate(RecordId rid, uint16_t length);

   const Slot* slotBegin() const;
   const Slot* slotEnd() const;
   Slot* slotBegin();
   Slot* slotEnd();

   SlottedPage() = delete; // Just in case you try :p
};

}

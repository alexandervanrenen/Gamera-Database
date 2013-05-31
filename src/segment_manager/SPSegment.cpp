#include "buffer_manager/BufferManager.hpp"
#include "FSISegment.hpp"
#include "Record.hpp"
#include "SegmentManager.hpp"
#include "SlottedPage.hpp"
#include "SPSegment.hpp"
#include "ExtentStore.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SPSegment::SPSegment(SegmentId id, FSISegment& fsi, SegmentInventory& si, BufferManager& bm)
: Segment(id, si, bm)
, freeSpaceInventory(fsi)
, fristFreePages({{beginPageID(), beginPageID(), beginPageID(), beginPageID(), beginPageID(), beginPageID(), beginPageID(), beginPageID(), beginPageID(), beginPageID()}})
{
}

SPSegment::~SPSegment()
{
}

TupleId SPSegment::insert(const Record& record)
{
   PageId pid = aquirePage(record.size());
   auto& frame = bufferManager.fixPage(pid, kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   RecordId rid = sp.insert(record);
   updateFreeBytes(pid, sp.getBytesFreeForRecord());
   bufferManager.unfixPage(frame, kDirty);
   return TupleId(pid, rid);
}

Record SPSegment::lookup(TupleId tid)
{
   assert(any_of(beginPageID(), endPageID(), [tid](const PageId& pid) {return pid==tid.toPageId();}));

   auto& frame = bufferManager.fixPage(tid.toPageId(), kShared);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   TupleId remoteId = sp.isReference(tid.toRecordId());
   if(remoteId == kInvalidTupleID) {
      Record result = sp.lookup(tid.toRecordId());
      bufferManager.unfixPage(frame, kClean);
      return result;
   }
   bufferManager.unfixPage(frame, kClean);
   return lookup(remoteId);
}

void SPSegment::remove(TupleId tId)
{
   assert(any_of(beginPageID(), endPageID(), [tId](const PageId& pid) {return pid==tId.toPageId();}));
   auto& frame = bufferManager.fixPage(tId.toPageId(), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   TupleId remoteTupleId = sp.isReference(tId.toRecordId());
   sp.remove(tId.toRecordId());
   bufferManager.unfixPage(frame, kDirty);
   if(remoteTupleId != kInvalidTupleID)
      remove(remoteTupleId);
}

TupleId SPSegment::insertForeigner(TupleId originalTupleId, const Record& record) {
   /// Find page and insert foreign record
   PageId pid = aquirePage(record.size() + sizeof(TupleId));
   auto& frame = bufferManager.fixPage(pid, kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   RecordId rid = sp.insertForeigner(record, originalTupleId);
   updateFreeBytes(pid, sp.getBytesFreeForRecord());
   bufferManager.unfixPage(frame, kDirty);
   return TupleId(pid, rid);
}

void SPSegment::update(TupleId tid, const Record& record)
{
   auto& frame = bufferManager.fixPage(tid.toPageId(), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   TupleId remoteId = sp.isReference(tid.toRecordId());

   // Case 1 - Record is on a single
   if(remoteId == kInvalidTupleID) {
      // Do simple in page update
      if(sp.canUpdateRecord(tid.toRecordId(), record)) {
         sp.update(tid.toRecordId(), record);
         updateFreeBytes(tid.toPageId(), sp.getBytesFreeForRecord());
         bufferManager.unfixPage(frame, kDirty);
         return;
      }
      // Store on some other page and add reference on original page
      TupleId remoteId = insertForeigner(tid, record);
      sp.updateToReference(tid.toRecordId(), remoteId);
      updateFreeBytes(tid.toPageId(), sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
      return;
   }

   // Case 2 -- Record is distributed across 2 pages
   if(sp.canUpdateRecord(tid.toRecordId(), record)) {
      // Move record back to first page
      sp.update(tid.toRecordId(), record);
      updateFreeBytes(tid.toPageId(), sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
      remove(remoteId);
      return;
   } else {
      // Update on second page
      auto& frame2 = bufferManager.fixPage(remoteId.toPageId(), kExclusive);
      auto& sp2 = reinterpret_cast<SlottedPage&>(*frame2.getData());
      if(sp2.canUpdateForeignRecord(remoteId.toRecordId(), record)) {
         // Update inside second page
         bufferManager.unfixPage(frame, kClean);
         sp2.updateForeigner(remoteId.toRecordId(), tid, record);
         updateFreeBytes(remoteId.toPageId(), sp2.getBytesFreeForRecord());
         bufferManager.unfixPage(frame2, kDirty);
         return;
      } else {
         // Remove from remote page (as it is to small)
         sp2.remove(remoteId.toRecordId());
         updateFreeBytes(remoteId.toPageId(), sp2.getBytesFreeForRecord());
         bufferManager.unfixPage(frame2, kDirty);

         // Store on some other page and add reference on original page
         TupleId remoteId = insertForeigner(tid, record);
         sp.updateToReference(tid.toRecordId(), remoteId);
         bufferManager.unfixPage(frame, kDirty);
         return;
      }
   }
   throw;
}

vector<pair<TupleId, Record>> SPSegment::getAllRecordsOfPage(PageId pageId)
{
   auto& frame = bufferManager.fixPage(pageId, kShared);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   auto result = sp.getAllRecords(pageId);
   bufferManager.unfixPage(frame, kClean);
   return result;
}

const Extent SPSegment::grow()
{
   Extent extent = Segment::grow();
   initializeExtent(extent);
   return extent;
}

const Extent SPSegment::grow(uint64_t numPages)
{
   Extent extent = Segment::grow(numPages);
   initializeExtent(extent);
   return extent;
}

void SPSegment::initializeExtent(Extent extent)
{
   // Get and initialize new extents
   for(PageId iter = extent.begin(); iter != extent.end(); iter++) {
      auto& frame = bufferManager.fixPage(iter, kExclusive);
      auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
      sp.initialize();
      updateFreeBytes(iter, sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
   }
}

uint32_t SPSegment::toLogScale(uint16_t bytes) const
{
   // Scale to log => (0,8)->0 .. (9,16)->1 .. (17,32)->2 ..
   uint32_t ld = 0;
   for(uint64_t tmp=bytes; tmp>1; tmp=tmp>>1)
      ld++;
   ld = ld>2 ? ld-3 : 0;
   ld = ld>=fristFreePages.size() ? fristFreePages.size()-1 : ld;
   return ld;
}

void SPSegment::updateFreeBytes(PageId pid, uint16_t freeBytes)
{
   // Update first free pages
   uint32_t ld = toLogScale(freeBytes);
   auto iter = findPageID(pid);
   for(uint32_t i=0; i<=ld; i++)
      fristFreePages[i] = min(fristFreePages[i], iter);

   // Update FSI
   freeSpaceInventory.setFreeBytes(pid, freeBytes);
}

PageId SPSegment::aquirePage(uint16_t length)
{
   for(uint32_t ld = toLogScale(length); ld<fristFreePages.size(); ld++)
      for(auto& iter = fristFreePages[ld]; iter != endPageID(); iter++)
         if(freeSpaceInventory.getFreeBytes(*iter) >= length)
            return *iter;

   grow();
   return aquirePage(length);
}

}

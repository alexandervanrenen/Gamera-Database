#include "buffer_manager/BufferManager.hpp"
#include "FSISegment.hpp"
#include "Record.hpp"
#include "SegmentManager.hpp"
#include "SlottedPage.hpp"
#include "SPSegment.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SPSegment::SPSegment(SegmentId id, SegmentManager& segmentManager, BufferManager& bufferManager, const ExtentStore& extents)
: Segment(id, bufferManager, extents)
, segmentManager(segmentManager)
{
}

SPSegment::~SPSegment()
{
}

void SPSegment::initializeExtent(const Extent& extent)
{
   assert(getNumPages() > 0);
   for(PageId iter = extent.begin(); iter != extent.end(); iter++) {
      auto& frame = bufferManager.fixPage(iter, kExclusive);
      auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
      sp.initialize();
      segmentManager.getFSISegment().setFreeBytes(iter, sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
   }
}

TId SPSegment::insert(const Record& record)
{
   PageId pid = aquirePage(record.size());
   auto& frame = bufferManager.fixPage(pid, kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   RecordId rid = sp.insert(record);
   segmentManager.getFSISegment().setFreeBytes(pid, sp.getBytesFreeForRecord());
   bufferManager.unfixPage(frame, kDirty);
   return toTID(pid, rid);
}

Record SPSegment::lookup(TId id)
{
   assert(any_of(beginPageID(), endPageID(), [id](const PageId& pid) {return pid==toPageId(id);}));

   auto& frame = bufferManager.fixPage(toPageId(id), kShared);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   TId remoteId = sp.isReference(toRecordId(id));
   if(remoteId == kInvalidTupleID) {
      Record result = sp.lookup(toRecordId(id));
      bufferManager.unfixPage(frame, kClean);
      return result;
   }
   bufferManager.unfixPage(frame, kClean);
   return lookup(remoteId);
}

void SPSegment::remove(TId tId)
{
   assert(any_of(beginPageID(), endPageID(), [tId](const PageId& pid) {return pid==toPageId(tId);}));
   auto& frame = bufferManager.fixPage(toPageId(tId), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   TId remoteTId = sp.isReference(toRecordId(tId));
   sp.remove(toRecordId(tId));
   bufferManager.unfixPage(frame, kDirty);
   if(remoteTId != kInvalidTupleID)
      remove(remoteTId);
}

TId SPSegment::insertForeigner(TId originalTId, const Record& record) {
   /// Find page and insert foreign record
   PageId pid = aquirePage(record.size() + sizeof(TId));
   auto& frame = bufferManager.fixPage(pid, kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   RecordId rid = sp.insertForeigner(record, originalTId);
   segmentManager.getFSISegment().setFreeBytes(pid, sp.getBytesFreeForRecord());
   bufferManager.unfixPage(frame, kDirty);
   return toTID(pid, rid);
}

void SPSegment::update(TId tid, const Record& record)
{
   auto& frame = bufferManager.fixPage(toPageId(tid), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   TId remoteId = sp.isReference(toRecordId(tid));

   // Case 1 - Record is on a single
   if(remoteId == kInvalidTupleID) {
      // Do simple in page update
      if(sp.canUpdateRecord(toRecordId(tid), record)) {
         sp.update(toRecordId(tid), record);
         segmentManager.getFSISegment().setFreeBytes(toPageId(tid), sp.getBytesFreeForRecord());
         bufferManager.unfixPage(frame, kDirty);
         return;
      }
      // Store on some other page and add reference on original page
      TId remoteId = insertForeigner(tid, record);
      sp.updateToReference(toRecordId(tid), remoteId);
      segmentManager.getFSISegment().setFreeBytes(toPageId(tid), sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
      return;
   }

   // Case 2 -- Record is distributed across 2 pages
   if(sp.canUpdateRecord(toRecordId(tid), record)) {
      // Move record back to first page
      sp.update(toRecordId(tid), record);
      segmentManager.getFSISegment().setFreeBytes(toPageId(tid), sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
      remove(remoteId);
      return;
   } else {
      // Update on second page
      auto& frame2 = bufferManager.fixPage(toPageId(remoteId), kExclusive);
      auto& sp2 = reinterpret_cast<SlottedPage&>(*frame2.getData());
      if(sp2.canUpdateForeignRecord(toRecordId(remoteId), record)) {
         // Update inside second page
         bufferManager.unfixPage(frame, kClean);
         sp2.updateForeigner(toRecordId(remoteId), tid, record);
         segmentManager.getFSISegment().setFreeBytes(toPageId(remoteId), sp2.getBytesFreeForRecord());
         bufferManager.unfixPage(frame2, kDirty);
         return;
      } else {
         // Remove from remote page (as it is to small)
         sp2.remove(toRecordId(remoteId));
         segmentManager.getFSISegment().setFreeBytes(toPageId(remoteId), sp2.getBytesFreeForRecord());
         bufferManager.unfixPage(frame2, kDirty);

         // Store on some other page and add reference on original page
         TId remoteId = insertForeigner(tid, record);
         sp.updateToReference(toRecordId(tid), remoteId);
         bufferManager.unfixPage(frame, kDirty);
         return;
      }
   }
   throw;
}

vector<pair<TId, Record>> SPSegment::getAllRecordsOfPage(PageId pageId)
{
   auto& frame = bufferManager.fixPage(pageId, kShared);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   auto result = sp.getAllRecords(pageId);
   bufferManager.unfixPage(frame, kClean);
   return result;
}

PageId SPSegment::aquirePage(uint16_t length)
{
   for(auto iter = beginPageID(); iter != endPageID(); iter++)
      if(segmentManager.getFSISegment().getFreeBytes(*iter) >= length)
         return *iter;
   segmentManager.growSegment(*this);
   return aquirePage(length);
}

}

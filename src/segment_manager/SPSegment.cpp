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

TupleId SPSegment::insert(const Record& record)
{
   PageId pid = aquirePage(record.size());
   auto& frame = bufferManager.fixPage(pid, kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   RecordId rid = sp.insert(record);
   segmentManager.getFSISegment().setFreeBytes(pid, sp.getBytesFreeForRecord());
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
   segmentManager.getFSISegment().setFreeBytes(pid, sp.getBytesFreeForRecord());
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
         segmentManager.getFSISegment().setFreeBytes(tid.toPageId(), sp.getBytesFreeForRecord());
         bufferManager.unfixPage(frame, kDirty);
         return;
      }
      // Store on some other page and add reference on original page
      TupleId remoteId = insertForeigner(tid, record);
      sp.updateToReference(tid.toRecordId(), remoteId);
      segmentManager.getFSISegment().setFreeBytes(tid.toPageId(), sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
      return;
   }

   // Case 2 -- Record is distributed across 2 pages
   if(sp.canUpdateRecord(tid.toRecordId(), record)) {
      // Move record back to first page
      sp.update(tid.toRecordId(), record);
      segmentManager.getFSISegment().setFreeBytes(tid.toPageId(), sp.getBytesFreeForRecord());
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
         segmentManager.getFSISegment().setFreeBytes(remoteId.toPageId(), sp2.getBytesFreeForRecord());
         bufferManager.unfixPage(frame2, kDirty);
         return;
      } else {
         // Remove from remote page (as it is to small)
         sp2.remove(remoteId.toRecordId());
         segmentManager.getFSISegment().setFreeBytes(remoteId.toPageId(), sp2.getBytesFreeForRecord());
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

PageId SPSegment::aquirePage(uint16_t length)
{
   for(auto iter = beginPageID(); iter != endPageID(); iter++)
      if(segmentManager.getFSISegment().getFreeBytes(*iter) >= length)
         return *iter;
   segmentManager.growSegment(*this);
   return aquirePage(length);
}

}

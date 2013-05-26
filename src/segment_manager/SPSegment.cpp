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
   pair<TId, Record> result = sp.lookup(toRecordId(id));
   bufferManager.unfixPage(frame, kClean);

   if(result.first == kInvalidTupleID)
      return move(result.second); else
      return lookup(result.first);
}

void SPSegment::remove(TId tId)
{
   assert(any_of(beginPageID(), endPageID(), [tId](const PageId& pid) {return pid==toPageId(tId);}));
   auto& frame = bufferManager.fixPage(toPageId(tId), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   sp.remove(toRecordId(tId));
   bufferManager.unfixPage(frame, kDirty);
}

void SPSegment::update(TId tid, const Record& record)
{
   auto& frame = bufferManager.fixPage(toPageId(tid), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   pair<TId, Record> result = sp.lookup(toRecordId(tid));

   // Record is on this page
   if(result.first == kInvalidTupleID) {
      if(sp.canUpdateRecord(toRecordId(tid), record)) {
         // Do simple in page update
         sp.update(toRecordId(tid), record);
         bufferManager.unfixPage(frame, kDirty);
         return;
      } else {
         // Store on some other page -- lets call it 2
         PageId pid2 = aquirePage(record.size());
         auto& frame2 = bufferManager.fixPage(pid2, kExclusive);
         auto& sp2 = reinterpret_cast<SlottedPage&>(*frame2.getData());
         RecordId rid = sp2.insertForeigner(record, tid);
         segmentManager.getFSISegment().setFreeBytes(pid2, sp2.getBytesFreeForRecord());
         bufferManager.unfixPage(frame2, kDirty);
         TId tid2 = toTID(pid2, rid);

         /// Add reference on original page to this page
         sp.updateToReference(toRecordId(tid), tid2);
         bufferManager.unfixPage(frame, kDirty);
         return;
      }
   } else {
      // Record is distributed across two pages -- fuck
      throw;
   }
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

#include "SPSegment.hpp"
#include "Record.hpp"
#include "SlottedPage.hpp"
#include "FSISegment.hpp"
#include "SegmentManager.hpp"
#include "buffer_manager/BufferManager.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SPSegment::SPSegment(SegmentId id, SegmentManager& segmentManager, BufferManager& bufferManager, const vector<Extent>& extents)
: Segment(id, bufferManager, extents)
, segmentManager(segmentManager)
{
}

SPSegment::~SPSegment()
{
}

void SPSegment::assignExtent(const Extent& extent)
{
   Segment::assignExtent(extent);
   for(PageId iter = extent.begin; iter != extent.end; iter++) {
      auto& frame = bufferManager.fixPage(iter, kExclusive);
      auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
      sp.initialize();
      segmentManager.getFSISegment().setFreeBytes(iter, sp.getBytesFreeForRecord());
      bufferManager.unfixPage(frame, kDirty);
   }
}

TId SPSegment::insert(const Record& record)
{
   // TODO: remember id of last insert and start iteration at this position
   for(auto iter = beginPageID(); iter != endPageID(); iter++) {
      if(segmentManager.getFSISegment().getFreeBytes(*iter) >= record.size()) {
         auto& frame = bufferManager.fixPage(*iter, kExclusive);
         auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
         RecordId id = sp.insert(record);
         segmentManager.getFSISegment().setFreeBytes(*iter, sp.getBytesFreeForRecord());
         bufferManager.unfixPage(frame, kDirty);
         return (*iter << 16) + id;
      }
   }

   // TODO: add ref to segment manager and grow
   cout << "no i am full !!!" << endl;
   throw;
}

Record SPSegment::lookup(TId id)
{
   assert(any_of(beginPageID(), endPageID(), [id](const PageId& pid) {return pid==toPageId(id);}));
   auto& frame = bufferManager.fixPage(toPageId(id), kShared);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   Record result = sp.lookup(toRecordId(id));
   bufferManager.unfixPage(frame, kClean);
   return result;
}

bool SPSegment::remove(TId tId)
{
   assert(any_of(beginPageID(), endPageID(), [tId](const PageId& pid) {return pid==toPageId(tId);}));
   auto& frame = bufferManager.fixPage(toPageId(tId), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   auto result = sp.remove(toRecordId(tId));
   bufferManager.unfixPage(frame, kDirty);
   return result;
}

TId SPSegment::update(TId tId, const Record& record)
{
   auto& frame = bufferManager.fixPage(toPageId(tId), kExclusive);
   auto& sp = reinterpret_cast<SlottedPage&>(*frame.getData());
   bool result = sp.tryInPageUpdate(toRecordId(tId), record);
   bufferManager.unfixPage(frame, kDirty);

   if(result) {
      return tId;
   } else {
      // TODO: maybe improve this: we unfix the page after tryUpdate though we fix it again afterwards to remove it
      remove(tId);
      return insert(record);
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

}

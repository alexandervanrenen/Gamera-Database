#pragma once

#include "Segment.hpp"
#include <vector>

namespace dbi {

class BufferManager;
class Record;
class SegmentManager;

class SPSegment : public Segment {
public:
   /// Constructor
   SPSegment(SegmentId id, SegmentManager& segmentManager, BufferManager& bufferManager, const ExtentStore& extents);
   virtual ~SPSegment();

   /// Called by segment manager after a extent has been added to this object
   virtual void initializeExtent(const Extent& extent);

   /// Operations on records
   TupleId insert(const Record& record);
   Record lookup(TupleId id);
   void remove(TupleId tId);
   void update(TupleId tId, const Record& record);
   std::vector<std::pair<TupleId, Record>> getAllRecordsOfPage(PageId pageId);

private:
   SegmentManager& segmentManager;

   /// Looks for a page in this segment large enough to hold length (grows if not found)
   PageId aquirePage(uint16_t length);
   /// Insert record on a new page
   TupleId insertForeigner(TupleId originalTupleId, const Record& record);
};

}

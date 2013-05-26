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
   TId insert(const Record& record);
   Record lookup(TId id);
   void remove(TId tId);
   void update(TId tId, const Record& record);
   std::vector<std::pair<TId, Record>> getAllRecordsOfPage(PageId pageId);

private:
   SegmentManager& segmentManager;
};

}

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
   SPSegment(SegmentId id, SegmentManager& segmentManager, BufferManager& bufferManager, const std::vector<Extent>& extents);
   virtual ~SPSegment();

   /// Add new extent to the segment (these pages need to be initialized for proper use)
   virtual void assignExtent(const Extent& extent);

   /// Operations on records
   TId insert(const Record& record);
   Record lookup(TId id);
   bool remove(TId tId);
   TId update(TId tId, const Record& record);
   std::vector<Record> getAllRecordsOfPage(PageId pId);

private:
   SegmentManager& segmentManager;
};

}

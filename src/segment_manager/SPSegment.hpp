#pragma once

#include "Segment.hpp"
#include <vector>

namespace dbi {

class FSISegment;
class BufferManager;
class Record;

class SPSegment : public Segment {
public:
   SPSegment(SegmentId id, FSISegment& freeSpaceInventory, BufferManager& bufferManager, const std::vector<Extent>& extents);
   virtual ~SPSegment() {}

   // Add new extent to the segment (these pages need to be initialized for proper use)
   virtual void assignExtent(const Extent& extent);

   // Add tuple to this segment .. awesome isn't it ?
   TId insert(const Record& record);

   // Lookup a tuple in this segment .. keeps getting better, e_y ?
   Record lookup(TId id);
   
   bool remove(TId tId);
   
   TId update(TId tId, Record& record);

private:
   FSISegment& freeSpaceInventory;
};

}

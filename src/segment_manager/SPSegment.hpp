#pragma once

#include "Segment.hpp"
#include <vector>

namespace dbi {

class FSISegment;
class BufferManager;
class Record;

class SPSegment : public Segment {
public:
   SPSegment(SegmentID id, FSISegment& freeSpaceInventory, BufferManager& bufferManager);
   virtual ~SPSegment() {}

   virtual void assignExtent(const Extent& extent);

   // Add tuple to this segment .. awesome isn't it ?
   TID insert(const Record& record);

   // Lookup a tuple in this segment .. keeps getting better, e_y ?
   Record lookup(TID id);

private:
   FSISegment& freeSpaceInventory;
};

}

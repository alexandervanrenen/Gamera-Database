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

   // Add tuple to this segment .. awesome isn't it ?
   TID insert(const Record& record);

private:
   FSISegment& freeSpaceInventory;
};

}

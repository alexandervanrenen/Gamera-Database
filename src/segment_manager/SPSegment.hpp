#pragma once

#include "Segment.hpp"
#include <vector>

namespace dbi {

class FSISegment;
class BufferManager;

class SPSegment : public Segment {
public:
   SPSegment(SegmentID id, std::vector<Extent> extents, FSISegment& freeSpaceInventory, BufferManager& bufferManager) : Segment(id, extents, bufferManager), freeSpaceInventory(freeSpaceInventory) {}

private:
   FSISegment& freeSpaceInventory;
};

}

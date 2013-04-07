#ifndef _SegmentManager_hpp
#define _SegmentManager_hpp

#include "SegmentType.hpp"

namespace dbi {

class BufferManager;

class SegmentManager {
public:
    SegmentManager(BufferManager& bufferManager);

    SegmentID createSegment(SegmentType segmentType, uint32_t totalSize);

    Segment getSegment(const SegmentID& id);

private:
    BufferManager& bufferManager;

    unique_ptr<SegmentInventory> segmentInventory;
};

}

#endif

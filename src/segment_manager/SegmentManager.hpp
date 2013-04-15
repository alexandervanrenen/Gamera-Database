#pragma once

#include "SegmentType.hpp"
#include "SegmentInventory.hpp"
#include <memory>

namespace dbi {

class BufferManager;
class Segment;

class SegmentManager {
public:
    SegmentManager(BufferManager& bufferManager);

    SegmentID createSegment(SegmentType segmentType, uint32_t totalSize);

    Segment& getSegment(const SegmentID& id);

private:
    BufferManager& bufferManager;

    SegmentInventory segmentInventory;
};

}

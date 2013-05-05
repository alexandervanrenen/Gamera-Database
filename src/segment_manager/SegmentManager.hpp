#pragma once

#include "common/Config.hpp"
#include "SegmentType.hpp"
#include "SegmentInventory.hpp"
#include <memory>
#include <unordered_map>

namespace dbi {

class BufferManager;
class SPSegment;

class SegmentManager {
public:
    SegmentManager(BufferManager& bufferManager);

    SegmentID createSegment(SegmentType segmentType, uint32_t numPages);

    SPSegment& getSPSegment(const SegmentID& id);

private:
    BufferManager& bufferManager;

    SegmentInventory segmentInventory;

    std::unordered_map<SegmentID, std::unique_ptr<SPSegment>> segments;
};

}

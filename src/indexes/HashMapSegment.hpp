#pragma once

#include "segment_manager/Segment.hpp"
#include "common/Config.hpp"

namespace dbi {

class HashMapSegment : public Segment {
public:
    HashMapSegment(SegmentId id, SegmentInventory& segmentInventory, BufferManager& bufferManager);

    virtual void initializeExtent(const Extent& extent);

    BufferFrame& getMetaBufferFrame() const;
    using Segment::fixGlobalPage;
    using Segment::unfixPage;
    using Segment::translateInternalToGlobalPageId;

    const Extent grow();
    const Extent grow(uint64_t numPages);
};

}

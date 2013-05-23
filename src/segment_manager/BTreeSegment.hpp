#pragma once

#include "BTreeMetadata.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "common/Config.hpp"
#include "Segment.hpp"
#include <vector>

namespace dbi {

const PageId metadataPage = 0;

class BufferManager;
class SegmentManager;

class BTreeSegment : public Segment {
public:
    /// Constructor
    BTreeSegment(SegmentId id, SegmentManager& sm, BufferManager& bufferManager, const std::vector<Extent>& extents);
    virtual ~BTreeSegment() {
        bufferManager.unfixPage(*metadataFrame, true);
    }

    /// Add new extent to the segment (these pages need to be initialized for proper use)
    virtual void assignExtent(const Extent& extent);

    BufferFrame& getPage(PageId id, bool exclusive=kShared);

    void releasePage(BufferFrame& frame, bool isDirty=false);
    
    std::pair<BufferFrame&, PageId> newPage();
    
    PageId getRootPage();
    void setRootPage(PageId id);

private:
    SegmentManager& segmentManager;
    BufferFrame* metadataFrame;
    BTreeMetadata* metadata;
};

}

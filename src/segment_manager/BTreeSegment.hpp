#pragma once

#include "BTreeMetadata.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "common/Config.hpp"
#include "Segment.hpp"
#include <vector>

namespace dbi {

constexpr PageId metadataPage = PageId(0);

class BufferManager;
class SegmentManager;

class BTreeSegment : public Segment {
public:
    /// Constructor
    BTreeSegment(SegmentId id, SegmentManager& sm, BufferManager& bufferManager, const ExtentStore& extents);
    virtual ~BTreeSegment() {
        bufferManager.unfixPage(*metadataFrame, true);
    }

    /// Called by segment manager after a extent has been added to this object
    virtual void initializeExtent(const Extent& extent);

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

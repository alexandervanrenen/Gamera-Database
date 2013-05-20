#pragma once

#include <vector>
#include "common/Config.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "Segment.hpp"
#include "BTreeMetadata.hpp"

namespace dbi {

const PageId metadataPage = 0;

class FSISegment;
class BufferManager;
class SegmentManager;

class BTreeSegment : public Segment {
public:
    /// Constructor
    BTreeSegment(SegmentId id, SegmentManager& sm, FSISegment& freeSpaceInventory, BufferManager& bufferManager, const std::vector<Extent>& extents);
    virtual ~BTreeSegment() {
        bufferManager.unfixPage(*metadataFrame, true);
    }

    /// Add new extent to the segment (these pages need to be initialized for proper use)
    virtual void assignExtent(const Extent& extent);

    BufferFrame& getPage(PageId id, bool exclusive=!kExclusive);

    void releasePage(BufferFrame& frame, bool isDirty=false);
    
    std::pair<BufferFrame&, PageId> newPage();
    
    PageId getRootPage();
    void setRootPage(PageId id);
private:
    FSISegment& freeSpaceInventory;
    SegmentManager& segmentManager;
    BufferFrame* metadataFrame;
    BTreeMetadata* metadata;
};

}

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
    BTreeSegment(SegmentId id, SegmentInventory& si, BufferManager& bm);
    virtual ~BTreeSegment();

    BufferFrame& getPage(PageId id, bool exclusive=kShared);

    void releasePage(BufferFrame& frame, bool isDirty=false);
    
    std::pair<BufferFrame&, PageId> newPage();
    
    PageId getRootPage();
    void setRootPage(PageId id);

    /// Get extents for this segment (extent is added by the segment inventory)
    virtual const Extent grow();
    virtual const Extent grow(uint64_t numPages);
};

}

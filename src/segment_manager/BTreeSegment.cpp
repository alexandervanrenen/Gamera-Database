#include "BTreeSegment.hpp"
#include "FSISegment.hpp"
#include "SegmentManager.hpp"
#include <iostream>

using namespace std;

namespace dbi {

BTreeSegment::BTreeSegment(SegmentId id, SegmentInventory& si, BufferManager& bm)
: Segment(id, si, bm)
{
    if (getNumPages() == 0) {
        Extent extent = Segment::grow();
        metadataFrame = &Segment::fixPage(metadataPage.toInteger(), kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->getData());
        metadata->nextFreePage = PageId(1);
        metadata->numberOfPages = extent.numPages();
        metadata->numberOfPages--; // minus metadatapage
        pair<BufferFrame&, PageId> p = newPage();
        bufferManager.unfixPage(p.first, true);
        metadata->rootPage = p.second;
    } else {
        metadataFrame = &Segment::fixPage(metadataPage.toInteger(), kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->getData());
    }
}

BufferFrame& BTreeSegment::getPage(PageId id, bool exclusive) {
    assert(id.toInteger() < metadata->nextFreePage.toInteger());
    return Segment::fixPage(id.toInteger(), exclusive);
}

void BTreeSegment::releasePage(BufferFrame& frame, bool isDirty) {
    bufferManager.unfixPage(frame, isDirty);
}

pair<BufferFrame&, PageId> BTreeSegment::newPage() {
    if (metadata->numberOfPages <= metadata->nextFreePage.toInteger()) {
        grow();
        assert(metadata->nextFreePage.toInteger() < metadata->numberOfPages);
    }
    PageId newid = metadata->nextFreePage++;
    return {Segment::fixPage(newid.toInteger(), kExclusive), newid};
}

PageId BTreeSegment::getRootPage() {
    return metadata->rootPage;
}

void BTreeSegment::setRootPage(PageId id) {
    metadata->rootPage = id;
}

const Extent BTreeSegment::grow()
{
   Extent extent = Segment::grow();
   metadata->numberOfPages += extent.numPages();
   return extent;
}

const Extent BTreeSegment::grow(uint64_t numPages)
{
   Extent extent = Segment::grow(numPages);
   metadata->numberOfPages += extent.numPages();
   return extent;
}

}

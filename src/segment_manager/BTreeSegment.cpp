#include "BTreeSegment.hpp"
#include "FSISegment.hpp"
#include "SegmentManager.hpp"
#include <cstring>
#include <iostream>

using namespace std;

namespace dbi {

BTreeSegment::BTreeSegment(SegmentId id, SegmentInventory& si, BufferManager& bm)
: Segment(id, si, bm)
{
    if (numPages() == 0) {
        Extent extent = Segment::grow();
        metadataFrame = &Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->data());
        metadata->nextFreePage = PageId(1);
        metadata->numberOfPages = extent.numPages();
        metadata->numberOfPages--; // minus metadatapage
        pair<BufferFrame&, PageId> p = newPage();
        unfixPage(p.first, true);
        metadata->rootPage = p.second;
    } else {
        metadataFrame = &Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->data());
    }
}

BTreeSegment::~BTreeSegment()
{
    unfixPage(*metadataFrame, true);
}

BufferFrame& BTreeSegment::getPage(PageId id, bool exclusive) {
    assert(id.toInteger() < metadata->nextFreePage.toInteger());
    return Segment::fixInternalPage(id.toInteger(), exclusive);
}

void BTreeSegment::releasePage(BufferFrame& frame, bool isDirty) {
    unfixPage(frame, isDirty);
}

pair<BufferFrame&, PageId> BTreeSegment::newPage() {
    if (metadata->numberOfPages <= metadata->nextFreePage.toInteger()) {
        grow();
        assert(metadata->nextFreePage.toInteger() < metadata->numberOfPages);
    }
    PageId newid = metadata->nextFreePage++;
    BufferFrame& bf = Segment::fixInternalPage(newid.toInteger(), kExclusive);
    memset(bf.data(), '\0', kPageSize);
    return {bf, newid};
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

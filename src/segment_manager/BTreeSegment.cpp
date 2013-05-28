#include "BTreeSegment.hpp"
#include "FSISegment.hpp"
#include "SegmentManager.hpp"
#include <iostream>

using namespace std;

namespace dbi {

BTreeSegment::BTreeSegment(SegmentId id, SegmentManager& sm, BufferManager& bufferManager, const ExtentStore& extents)
: Segment(id, bufferManager, extents)
, segmentManager(sm)
{
    if (getNumPages() > 0) {
        metadataFrame = &getPage(metadataPage, kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->getData());
    } 
}

void BTreeSegment::initializeExtent(const Extent& extent) {
    //std::cout << "Assigning extent with begin: " << extent.begin << ", end: " << extent.end << std::endl;
    if (getNumPages() == 0) { // first extent -> initialize metadata
        metadataFrame = &Segment::fixPage(metadataPage.toInteger(), kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->getData());
        metadata->nextFreePage = PageId(1);
        metadata->numberOfPages = extent.numPages();
        metadata->numberOfPages--; // minus metadatapage
        pair<BufferFrame&, PageId> p = newPage();
        bufferManager.unfixPage(p.first, true);
        metadata->rootPage = p.second;
    } else {
        metadata->numberOfPages += extent.numPages();
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
        segmentManager.growSegment(*this);
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

}


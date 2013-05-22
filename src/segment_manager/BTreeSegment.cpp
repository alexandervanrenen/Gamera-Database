#include "BTreeSegment.hpp"
#include "SegmentManager.hpp"
#include "FSISegment.hpp"
#include <iostream>

using namespace std;

namespace dbi {

BTreeSegment::BTreeSegment(SegmentId id, SegmentManager& sm, BufferManager& bufferManager, const vector<Extent>& extents)
: Segment(id, bufferManager, extents)
, segmentManager(sm)
{
    if (getNumPages() > 0) {
        metadataFrame = &getPage(metadataPage, kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->getData());
    } 
}

void BTreeSegment::assignExtent(const Extent& extent) {
    //std::cout << "Assigning extent with begin: " << extent.begin << ", end: " << extent.end << std::endl;
    if (getNumPages() == 0) { // first extent -> initialize metadata
        Segment::assignExtent(extent);
        metadataFrame = &Segment::fixPage(metadataPage, kExclusive);
        metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame->getData());
        metadata->nextFreePage = 1;
        metadata->numberOfPages = extent.numPages();
        metadata->numberOfPages--; // minus metadatapage
        pair<BufferFrame&, PageId> p = newPage();
        bufferManager.unfixPage(p.first, true);
        metadata->rootPage = p.second;
    } else {
        Segment::assignExtent(extent);
        metadata->numberOfPages += extent.numPages();

    }
}


BufferFrame& BTreeSegment::getPage(PageId id, bool exclusive) {
    assert(id < metadata->nextFreePage);
    return Segment::fixPage(id, exclusive);
}

void BTreeSegment::releasePage(BufferFrame& frame, bool isDirty) {
    bufferManager.unfixPage(frame, isDirty);
}


pair<BufferFrame&, PageId> BTreeSegment::newPage() {
    if (metadata->numberOfPages <= metadata->nextFreePage) {
        segmentManager.growSegment(*this);
        assert(metadata->nextFreePage < metadata->numberOfPages);
    }
    PageId newid = metadata->nextFreePage++;
    return {Segment::fixPage(newid, kExclusive), newid};
}

PageId BTreeSegment::getRootPage() {
    return metadata->rootPage;
}

void BTreeSegment::setRootPage(PageId id) {
    metadata->rootPage = id;
}

}


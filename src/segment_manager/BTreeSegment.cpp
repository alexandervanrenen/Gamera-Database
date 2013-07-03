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
        auto& metadataFrame = Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
        auto metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame.data());
        metadata->nextFreePage = PageId(1);
        metadata->numberOfPages = extent.numPages();
        metadata->numberOfPages--; // minus metadatapage
        unfixPage(metadataFrame, true);

        pair<BufferFrame&, PageId> p = newPage();
        unfixPage(p.first, true);
        setRootPage(p.second);
    }
}

BTreeSegment::~BTreeSegment()
{
}

BufferFrame& BTreeSegment::getPage(PageId id, bool exclusive) 
{
    auto& metadataFrame = Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
    auto metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame.data());
    assert(id.toInteger() < metadata->nextFreePage.toInteger());
    unfixPage(metadataFrame, true);
    return Segment::fixInternalPage(id.toInteger(), exclusive);
}

void BTreeSegment::releasePage(BufferFrame& frame, bool isDirty)
{
    unfixPage(frame, isDirty);
}

pair<BufferFrame&, PageId> BTreeSegment::newPage()
{
    auto& metadataFrame = Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
    auto metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame.data());

    if (metadata->numberOfPages <= metadata->nextFreePage.toInteger()) {
        grow();
        assert(metadata->nextFreePage.toInteger() < metadata->numberOfPages);
    }
    PageId newid = metadata->nextFreePage++;
    BufferFrame& bf = Segment::fixInternalPage(newid.toInteger(), kExclusive);
    memset(bf.data(), '\0', kPageSize);

    unfixPage(metadataFrame, true);
    return {bf, newid};
}

PageId BTreeSegment::getRootPage()
{
    auto& metadataFrame = Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
    auto metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame.data());

    auto pid = metadata->rootPage;

    unfixPage(metadataFrame, false);
    return pid;
}

void BTreeSegment::setRootPage(PageId id)
{
    auto& metadataFrame = Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
    auto metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame.data());

    metadata->rootPage = id;

    unfixPage(metadataFrame, true);
}

const Extent BTreeSegment::grow()
{
    auto& metadataFrame = Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
    auto metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame.data());

    Extent extent = Segment::grow();
    metadata->numberOfPages += extent.numPages();

    unfixPage(metadataFrame, true);
    return extent;
}

const Extent BTreeSegment::grow(uint64_t numPages)
{
    auto& metadataFrame = Segment::fixInternalPage(metadataPage.toInteger(), kExclusive);
    auto metadata = reinterpret_cast<BTreeMetadata*>(metadataFrame.data());

    Extent extent = Segment::grow(numPages);
    metadata->numberOfPages += extent.numPages();

    unfixPage(metadataFrame, true);
    return extent;
}

}

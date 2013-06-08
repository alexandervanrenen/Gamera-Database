#include "HashMapSegment.hpp"
#include "HashMapMetaPage.hpp"
#include "HashMapDirectoryPage.hpp"
#include "HashMapBucketPage.hpp"
#include "buffer_manager/BufferManager.hpp"

using namespace std;

namespace dbi {

HashMapSegment::HashMapSegment(SegmentId id, SegmentInventory& segmentInventory, BufferManager& bufferManager)
: Segment(id, segmentInventory, bufferManager)
{
   if(numPages() == 0) {
      // Initial setup -- one meta page (pid=0), one directory page (pid=1) with two buckets (pid=2,3)
      grow(10);

      // Set up meta page
      auto& metaBf = fixInternalPage(0, kExclusive);
      auto& metaPage = reinterpret_cast<HashMapMetaPage&>(*metaBf.data());
      PageId firstDirectoryPage = translateInternalToGlobalPageId(2)
      metaPage.initialize(Extent(firstDirectoryPage, PageId(firstDirectoryPage.toInteger()+1)));
      unfixPage(metaBf, kDirty);

      // Set up directory
      auto& directoryBf = fixGlobalPage(firstDirectoryPage, kExclusive);
      auto& directoryPage = reinterpret_cast<DirectoryPage&>(*directoryBf.data());
      directoryPage[0] = translateInternalToGlobalPageId(2);
      directoryPage[1] = translateInternalToGlobalPageId(3);
      unfixPage(directoryBf, kDirty);

      // Set up buckets
      auto& firstBucketBf = fixInternalPage(2, kExclusive);
      auto& firstBucketPage = reinterpret_cast<HashMapBucketPage<int,int>&>(*firstBucketBf.data()); // Just use any template parameter, as we do not know the right ones and it does not matter for the initialize method
      firstBucketPage.initialize();
      unfixPage(firstBucketBf, kDirty);

      auto& secondBucketBf = fixInternalPage(3, kExclusive);
      auto& secondBucketPage = reinterpret_cast<HashMapBucketPage<int,int>&>(*secondBucketBf.data());
      secondBucketPage.initialize();
      unfixPage(secondBucketBf, kDirty);

   } else {
      // Restart
      throw;
   }
}

void HashMapSegment::initializeExtent(const Extent& extent)
{
}

BufferFrame& HashMapSegment::getMetaBufferFrame() const
{
   return fixInternalPage(0, kExclusive);
}

const Extent HashMapSegment::grow()
{
   Extent extent = Segment::grow();
   return extent;
}

const Extent HashMapSegment::grow(uint64_t numPages)
{
   Extent extent = Segment::grow(numPages);
   return extent;
}

}

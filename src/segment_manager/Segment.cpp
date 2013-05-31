#include "buffer_manager/BufferManager.hpp"
#include "Segment.hpp"
#include "ExtentStore.hpp"
#include "SegmentInventory.hpp"
#include <cassert>

using namespace std;

namespace dbi {

Segment::Segment(SegmentId id, SegmentInventory& segmentInventory, BufferManager& bufferManager)
: id(id)
, extents(segmentInventory.getExtentsOfSegment(id))
, segmentInventory(segmentInventory)
, bufferManager(bufferManager)
{
}

uint64_t Segment::numPages() const
{
   return extents.numPages();
}

PageIDIterator Segment::beginPageID()
{
   return PageIDIterator(extents.get(), extents.get().size() == 0 ? kInvalidPageID : extents.get()[0].begin());
}

PageIDIterator Segment::findPageID(PageId pid)
{
   return PageIDIterator(extents.get(), pid);
}

PageIDIterator Segment::endPageID()
{
   return PageIDIterator(extents.get(), kInvalidPageID);
}

const Extent Segment::grow()
{
   return segmentInventory.growSegment(id);
}

const Extent Segment::grow(uint64_t numPages)
{
   return segmentInventory.growSegment(id, numPages);
}

BufferFrame& Segment::fixInternalPage(uint64_t offset, bool exclusive) const
{
   assert(extents.get().size() != 0);

   // Find extent for the requested offset
   for(auto& iter : extents.get())
      if(iter.numPages() > offset)
         return bufferManager.fixPage(PageId(iter.begin().toInteger() + offset), exclusive);
      else
         offset -= iter.numPages();

   assert(false && "offset not in segment");
   throw;
}

BufferFrame& Segment::fixGlobalPage(PageId pid, bool exclusive) const
{
   bufferManager.fixPage(pid, exclusive);
}

void Segment::unfixPage(BufferFrame& bufferFrame, bool dirty) const
{
   bufferManager.unfixPage(bufferFrame, dirty);
}

}

#include "buffer_manager/BufferManager.hpp"
#include "Segment.hpp"
#include "ExtentStore.hpp"
#include <cassert>

using namespace std;

namespace dbi {

Segment::Segment(SegmentId id, BufferManager& bufferManager, const ExtentStore& extents)
: id(id)
, extents(extents)
, bufferManager(bufferManager)
{
}

uint64_t Segment::getNumPages() const
{
   return extents.numPages();
}

PageIDIterator Segment::beginPageID()
{
   return PageIDIterator(extents.get(), extents.get().size() == 0 ? kInvalidPageID : extents.get()[0].begin());
}

PageIDIterator Segment::endPageID()
{
   return PageIDIterator(extents.get(), kInvalidPageID);
}

BufferFrame& Segment::fixPage(uint64_t offset, bool exclusive) const
{
   assert(extents.get().size() != 0);

   // Find extent for the requested offset
   for(auto& iter : extents.get())
      if(iter.numPages() > offset)
         return bufferManager.fixPage(iter.begin() + offset, exclusive);
      else
         offset -= iter.numPages();

   assert(false && "offset not in segment");
   throw;
}

void Segment::unfixPage(BufferFrame& bufferFrame, bool dirty) const
{
   bufferManager.unfixPage(bufferFrame, dirty);
}

}

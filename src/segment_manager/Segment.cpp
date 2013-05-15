#include "Segment.hpp"
#include "buffer_manager/BufferManager.hpp"
#include <cassert>

using namespace std;

namespace dbi {

Segment::Segment(SegmentId id, BufferManager& bufferManager, const vector<Extent>& extents)
: id(id)
, extents(extents)
, numPages(std::accumulate(extents.begin(),extents.end(),(uint64_t)0,[](uint64_t count, const Extent& extent) {return count+extent.numPages();}))
, bufferManager(bufferManager)
{
}

void Segment::assignExtent(const Extent& extent)
{
   numPages+=extent.numPages();
   for(auto& iter : extents)
      if(extent.end == iter.begin || extent.begin == iter.end) {
         iter = Extent{std::min(extent.begin, iter.begin), std::max(extent.end, iter.end)};
         return;
      }
   extents.emplace_back(extent);
}

BufferFrame& Segment::fixPage(uint64_t offset, bool exclusive) const
{
   assert(extents.size() != 0);

   // Find extent for the requested offset
   for(auto& iter : extents)
      if(iter.numPages() > offset)
         return bufferManager.fixPage(iter.begin+offset, exclusive); else
         offset -= iter.numPages();

   assert(false && "offset not in segment");
   throw;
}

void Segment::unfixPage(BufferFrame& bufferFrame, bool dirty) const
{
   bufferManager.unfixPage(bufferFrame, dirty);
}

}

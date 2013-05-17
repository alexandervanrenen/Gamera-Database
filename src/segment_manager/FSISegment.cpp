#include "FSISegment.hpp"
#include "buffer_manager/BufferFrame.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

FSISegment::FSISegment(SegmentId id, BufferManager& bufferManager, const std::vector<Extent>& extents)
: Segment(id, bufferManager, extents)
{
}

void FSISegment::assignExtent(const Extent& extent)
{
   Segment::assignExtent(extent);
}

uint32_t FSISegment::getFreeBytes(PageId id) const
{
   // Get data
   uint32_t extent = id / 2 / kPageSize; // which extent to look on
   BufferFrame& bufferFrame = fixPage(extent, false);
   char* data = bufferFrame.getData();

   // Read nibble
   uint32_t offset = id / 2 % kPageSize; // offset on page
   uint32_t result = data[offset];
   if(id % 2 == 0) {
      result &= 0x0F;
   } else {
      result &= 0xF0;
      result >>= 4;
   }

   unfixPage(bufferFrame, false);
   return result * kPageSize / 16;
}

void FSISegment::setFreeBytes(PageId id, uint32_t freeBytes)
{
   // Get data
   uint32_t extent = id / 2 / kPageSize; // which extent to look on
   BufferFrame& bufferFrame = fixPage(extent, true);
   char* data = bufferFrame.getData();

   // Change nibble
   uint32_t offset = id / 2 % kPageSize; // offset on page
   uint8_t val = freeBytes * 16 / kPageSize; // encode free bytes
   if(id % 2 == 0) {
      data[offset] &= 0xF0;
      data[offset] |= val;
   } else {
      data[offset] &= 0x0F;
      data[offset] |= (val << 4);
   }
   unfixPage(bufferFrame, true);
}

}

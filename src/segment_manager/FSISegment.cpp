#include "buffer_manager/BufferFrame.hpp"
#include "FSISegment.hpp"
#include "buffer_manager/BufferManager.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

FSISegment::FSISegment(SegmentId id, SegmentInventory& segmentInventory, BufferManager& bufferManager)
: Segment(id, segmentInventory, bufferManager)
{
   // Calculate required pages for free space inventory
   uint64_t FSIBytes = (bufferManager.getNumDiscPages() + 1) / 2; // Required bytes
   uint64_t FSIPages = FSIBytes / kPageSize + (FSIBytes % kPageSize != 0); // Required pages
   Segment::grow(FSIPages);
}

FSISegment::~FSISegment()
{
}

uint32_t FSISegment::getFreeBytes(PageId id) const
{
   // Get data
   uint32_t extent = id.toInteger() / 2 / kPageSize; // which extent to look on
   BufferFrame& bufferFrame = fixPage(extent, false);
   char* data = bufferFrame.data();

   // Read nibble
   uint32_t offset = id.toInteger() / 2 % kPageSize; // offset on page
   uint32_t result = data[offset];
   if(id.toInteger() % 2 == 0) {
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
   uint32_t extent = id.toInteger() / 2 / kPageSize; // which extent to look on
   BufferFrame& bufferFrame = fixPage(extent, true);
   char* data = bufferFrame.data();

   // Change nibble
   uint32_t offset = id.toInteger() / 2 % kPageSize; // offset on page
   uint8_t val = freeBytes * 16 / kPageSize; // encode free bytes
   if(id.toInteger() % 2 == 0) {
      data[offset] &= 0xF0;
      data[offset] |= val;
   } else {
      data[offset] &= 0x0F;
      data[offset] |= (val << 4);
   }
   unfixPage(bufferFrame, true);
}

const Extent FSISegment::grow()
{
   assert("growing segment inventory"&&false);
   throw;
}

const Extent FSISegment::grow(uint64_t)
{
   assert("growing segment inventory"&&false);
   throw;
}

}

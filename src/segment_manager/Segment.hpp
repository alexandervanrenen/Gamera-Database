#pragma once

#include "common/Config.hpp"
#include "PageIDIterator.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

namespace dbi {

class BufferManager;
class BufferFrame;
class ExtentStore;
class Extent;
class SegmentInventory;

class Segment {
public:
   Segment(SegmentId id, SegmentInventory& segmentInventory, BufferManager& bufferManager);
   virtual ~Segment() {}

   SegmentId getId() const {return id;}

   uint64_t numPages() const;

   /// Iterate over all pages in segment -- Do not change segment while iterating ..
   PageIDIterator beginPageID() const;
   PageIDIterator findPageID(PageId pid) const;
   PageIDIterator endPageID() const;

   /// Get extents for this segment (extent is added by the segment inventory)
   virtual const Extent grow() = 0;
   virtual const Extent grow(uint64_t numPages) = 0;

private:
   const SegmentId id;
   const ExtentStore& extents;
   SegmentInventory& segmentInventory;
   BufferManager& bufferManager;

protected:
   /// Assumes internal address space (i.E. extents[0].begin + offset)
   BufferFrame& fixGlobalPage(PageId pid, bool exclusive) const;
   BufferFrame& fixInternalPage(uint64_t offset, bool exclusive) const;
   void unfixPage(BufferFrame& bufferFrame, bool dirty) const;
};

}

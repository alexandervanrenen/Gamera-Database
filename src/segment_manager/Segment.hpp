#pragma once

#include "common/Config.hpp"
#include "PageIDIterator.hpp"
#include "Extent.hpp"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

namespace dbi {

class BufferManager;
class BufferFrame;

class Segment {
public:
   Segment(SegmentId id, BufferManager& bufferManager, const std::vector<Extent>& extents);
   virtual ~Segment() {}

   SegmentId getId() const {return id;}

   uint64_t getNumPages() const {return numPages;}

   /// Add new extent to the segment (these pages need to be initialized for proper use)
   virtual void assignExtent(const Extent& extent) = 0;

   /// Iterate over all pages in segment -- Do not change segment while iterating ..
   PageIDIterator beginPageID() {return PageIDIterator(extents, extents.size()==0?kInvalidPageID:extents[0].begin);}
   PageIDIterator endPageID() {return PageIDIterator(extents, kInvalidPageID);}

private:
   const SegmentId id;
   std::vector<Extent> extents;
   uint64_t numPages;

protected:
   /// Assumes internal address space (i.E. extents[0].begin + offset)
   BufferFrame& fixPage(uint64_t offset, bool exclusive) const;
   void unfixPage(BufferFrame& bufferFrame, bool dirty) const;
   /// Or use buffer manager to directly fix the page
   BufferManager& bufferManager;
};

}

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

class Segment {
public:
   Segment(SegmentId id, BufferManager& bufferManager, const ExtentStore& extents);
   virtual ~Segment() {}

   SegmentId getId() const {return id;}

   uint64_t getNumPages() const;

   /// Called by segment manager after a extent has been added to this object
   virtual void initializeExtent(const Extent& extent) = 0;

   /// Iterate over all pages in segment -- Do not change segment while iterating ..
   PageIDIterator beginPageID();
   PageIDIterator endPageID();

private:
   const SegmentId id;
   const ExtentStore& extents;

protected:
   /// Assumes internal address space (i.E. extents[0].begin + offset)
   BufferFrame& fixPage(uint64_t offset, bool exclusive) const; // TODO: no direct access to buffer manager
   void unfixPage(BufferFrame& bufferFrame, bool dirty) const;
   /// Or use buffer manager to directly fix the page
   BufferManager& bufferManager;
};

}

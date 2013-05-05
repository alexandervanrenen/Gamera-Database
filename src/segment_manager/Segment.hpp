#pragma once

#include "common/Config.hpp"
#include "Extent.hpp"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <iostream>

namespace dbi {

class BufferManager;
class BufferFrame;

class Segment {
public:
   Segment(SegmentID id, std::vector<Extent> extents, BufferManager& bufferManager);

   SegmentID getId() const {return id;}

   uint64_t getNumPages() const {return numPages;}

   void addExtent(const Extent& extent);

private:
   SegmentID id;
   std::vector<Extent> extents;
   uint64_t numPages;
   BufferManager& bufferManager;

protected:
   /// Assumes internal address space (i.E. extents[0].begin + offset)
   BufferFrame& fixPage(uint64_t offset, bool exclusive) const;
   void unfixPage(BufferFrame& bufferFrame, bool dirty) const;
};

}

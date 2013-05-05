#pragma once

#include "common/Config.hpp"
#include "Extent.hpp"
#include <cstdint>
#include <vector>
#include <algorithm>

namespace dbi {

class Segment {
public:
    Segment(SegmentID id, std::vector<Extent> extents) : id(id), extents(extents), numPages(std::accumulate(extents.begin(),extents.end(),(uint64_t)0,[](uint64_t count, const Extent& extent){return count+extent.numPages();})) {}

    SegmentID getId() const {return id;}

    uint64_t getNumPages() const {return numPages;}

    void addExtent(const Extent& extent) {extents.push_back(extent); numPages+=extent.numPages();}

private:
   SegmentID id;
   std::vector<Extent> extents;
   uint64_t numPages;
};

}

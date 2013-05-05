#pragma once

#include "common/Config.hpp"
#include "Extent.hpp"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <iostream>

namespace dbi {

class Segment {
public:
    Segment(SegmentID id, std::vector<Extent> extents) : id(id), extents(extents), numPages(std::accumulate(extents.begin(),extents.end(),(uint64_t)0,[](uint64_t count, const Extent& extent){return count+extent.numPages();})) {}

    SegmentID getId() const {return id;}

    uint64_t getNumPages() const {return numPages;}

    void addExtent(const Extent& extent) {numPages+=extent.numPages(); for(auto& iter : extents) if(extent.end == iter.begin || extent.begin == iter.end) {iter = Extent{std::min(extent.begin, iter.begin), std::max(extent.end, iter.end)}; return;} extents.emplace_back(extent);}

private:
   SegmentID id;
   std::vector<Extent> extents;
   uint64_t numPages;
};

}

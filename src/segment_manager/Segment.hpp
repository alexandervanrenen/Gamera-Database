#pragma once

#include "common/Config.hpp"
#include "Extent.hpp"
#include <cstdint>
#include <vector>

namespace dbi {

class Segment {
public:
    Segment(SegmentID id, std::vector<Extent> extends) : id(id), extends(extends) {}

    SegmentID getId() {return id;}

private:
   SegmentID id;
   std::vector<Extent> extends;
};

}

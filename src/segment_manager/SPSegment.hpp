#pragma once

#include "Segment.hpp"
#include <vector>

namespace dbi {

class SPSegment : public Segment {
public:
    SPSegment(SegmentID id, std::vector<Extent> extends) : Segment(id, extends) {}
};

}

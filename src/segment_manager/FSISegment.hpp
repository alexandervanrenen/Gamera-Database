#pragma once

#include "Segment.hpp"

namespace dbi {

class FSISegment : public Segment {
public:
   FSISegment(SegmentID id, std::vector<Extent> extends) : Segment(id, extends) {}

};

}

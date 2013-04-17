#pragma once

#include <cstdint>
#include "SegmentType.hpp"

namespace dbi {

class Segment {
public:
    Segment();

    uint32_t size();

    SegmentID id;
};

}

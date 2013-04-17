#pragma once

#include "Segment.hpp"

namespace dbi {

class SegmentInventory : public Segment {

   void create();

   void drop();

   void grow();
};

}

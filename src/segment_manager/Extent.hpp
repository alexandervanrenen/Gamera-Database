#pragma once

#include "common/Config.hpp"

namespace dbi {

struct Extent {
   uint64_t numPages() const {return end-begin;}

   // Pages belonging to this extent: [begin, end)
   PageId begin;
   PageId end;
};

}
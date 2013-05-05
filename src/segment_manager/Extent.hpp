#pragma once

#include "common/Config.hpp"

namespace dbi {

struct Extent {
   uint64_t numPages() {return begin-end;}

   // Pages belonging to this extent: [begin, end)
   PageID begin;
   PageID end;
};

}
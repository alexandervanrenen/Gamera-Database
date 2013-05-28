#pragma once

#include "common/Config.hpp"

namespace dbi {

struct BTreeMetadata {
   PageId rootPage = PageId(0);
   PageId nextFreePage = PageId(1);
   uint64_t numberOfPages = 0;
};

}

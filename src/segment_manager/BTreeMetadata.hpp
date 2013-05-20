#pragma once

#include "common/Config.hpp"

namespace dbi {

struct BTreeMetadata {
   PageId rootPage = 0;
   PageId nextFreePage = 1;
   uint64_t numberOfPages = 0;
};

}

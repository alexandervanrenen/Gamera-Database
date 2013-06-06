#pragma once

#include "common/Config.hpp"
#include <cstdint>

namespace dbi {

class DirectoryPage {
public:
   PageId& operator[] (uint32_t pos)
   {
      assert(pos < kPageSize / sizeof(PageId));
      return *((&entry)+pos);
   }

private:
   PageId entry;
};

}

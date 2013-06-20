#pragma once

#include <cstdint>
#include <string>

namespace dbi {

struct DatabaseConfig {
   /// The file used by the buffer manager to load and save pages
   std::string storageFile;
   /// The number of pages the buffer manager is allowed to use
   uint64_t memoryPages;
};

}

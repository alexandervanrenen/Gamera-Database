#ifndef __Utility
#define __Utility

#include <stdint.h>
#include <functional>
#include <utility>
#include <fstream>
#include <string>
#include <vector>

namespace dbi_utility {

/// Create a file with count entries created by the given factory function
bool createTestFile(const std::string& fileName, uint64_t count, std::function<
      uint64_t(uint64_t)> factory);

/// Read a file and invoke callback function on every entry
bool foreachInFile(const std::string& fileName, std::function<void(uint64_t)> callback);

}

#endif

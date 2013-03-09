#ifndef __ExternalSort
#define __ExternalSort

#include <string>
#include <stdint.h>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>

namespace dbi {

class ExternalSort {
public:
   ExternalSort();

   /// Assumes enough available memory
   void simpleSort(const std::string& inputFileName, const std::string& outputFileName);

   /// For huge files, takes up only the specified amount of memory
   void complexSort(const std::string& fileName, uint64_t maxMemory, uint64_t pageSize);
};

}

#endif

#ifndef __ExternalSort
#define __ExternalSort

#include <string>
#include <cstdint>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>

namespace dbi {

class ExternalSort {
public:
  ExternalSort();

  void simpleSort(const std::string& inputFileName, const std::string& outputFileName);
};

}

#endif

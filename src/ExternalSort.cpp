#include "ExternalSort.hpp"
#include <fstream>

namespace dbi {

using namespace std;

ExternalSort::ExternalSort(const string& fileName)
: fileName(fileName)
{
}

void ExternalSort::sort()
{
}
  
unique_ptr<uint64_t*> ExternalSort::read(uint32_t /*begin*/, uint32_t count)
{
  malloc(count);
  return nullptr;
}

}

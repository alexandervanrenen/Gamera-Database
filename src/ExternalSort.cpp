#include "ExternalSort.hpp"
#include <fstream>

using namespace std;

ExternalSort::ExternalSort(const string& fileName)
: fileName(fileName)
{
}
  
unique_ptr<uint64_t*> ExternalSort::read(uint32_t begin, uint32_t count)
{
  malloc(count);
}

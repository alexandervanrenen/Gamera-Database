#include "ExternalSort.hpp"
#include <fstream>

namespace dbi {

using namespace std;

ExternalSort::ExternalSort()
{
}

template<class T>
void simpleSortImpl(const std::string& inputFileName, const std::string& outputFileName)
{
   // Open file
   std::ifstream in(inputFileName, std::ios::binary);
   if (!in.is_open() || !in.good())
      throw;

   // Figure out file length and entry count
   in.seekg(0, std::ios::end);
   uint64_t fileLength = static_cast<uint64_t>(in.tellg());
   in.seekg(0, std::ios::beg);
   if (fileLength % sizeof(T) != 0)
      throw;
   uint64_t entryCount = fileLength / sizeof(T);

   // Read into buffer
   std::vector<T> buffer(entryCount);
   in.read(reinterpret_cast<char*>(buffer.data()), fileLength);

   // Sort
   std::sort(buffer.begin(), buffer.end());

   // Open output and write data
   std::ofstream out(outputFileName, std::ios::binary);
   if (!out.is_open() || !out.good())
      throw;
   out.write(reinterpret_cast<char*>(buffer.data()), fileLength);
}

void ExternalSort::simpleSort(const std::string& inputFileName, const std::string& outputFileName)
{
   simpleSortImpl<uint64_t>(inputFileName, outputFileName);
}

void ExternalSort::complexSort(const std::string& /*inputFileName*/, const std::string& /*outputFileName*/, uint64_t /*maxMemory*/)
{

}

}

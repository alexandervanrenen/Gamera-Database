#include "ExternalSort.hpp"
#include <fstream>
#include <iostream>

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

template<class T>
void complexSortImpl(const std::string& fileName, uint64_t maxMemory)
{
   vector<T> buffer(maxMemory / sizeof(T));
   const uint64_t bufferEntrySize = buffer.size() * sizeof(T);

   // Read file sequentially, creating runs
   fstream in(fileName, ios::binary | ios::out | ios::in);
   while (true) {
      auto pos = in.tellg();
      in.read(reinterpret_cast<char*>(buffer.data()), bufferEntrySize);
      auto readBytes = in.tellg() - pos;
      cout << readBytes << endl;
      if (readBytes <= 0)
         break;
      sort(buffer.begin(), buffer.begin() + readBytes / sizeof(T));
      in.seekg(-readBytes, ios::cur);
      in.write(reinterpret_cast<char*>(buffer.data()), readBytes);
   }
}

void ExternalSort::simpleSort(const std::string& inputFileName, const std::string& outputFileName)
{
   simpleSortImpl<uint64_t>(inputFileName, outputFileName);
}

void ExternalSort::complexSort(const std::string& fileName, uint64_t maxMemory)
{
   complexSortImpl<uint64_t>(fileName, maxMemory);
}

}

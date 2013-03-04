#include "Utility.hpp"

namespace dbi_utility {

template<class T>
bool createTestFileImpl(const std::string& fileName, uint64_t count, std::function<
      T(int)> factory)
{
   // Open file
   std::ofstream of(fileName, std::ios::binary);
   if (!of.is_open() || !of.good())
      return false;

   // Write file in buffered fashion
   const uint32_t kMaxBufferSize = 1 << 22;
   std::vector<T> buffer(kMaxBufferSize / sizeof(T));
   for (uint64_t i = 0; i < count;) {
      // Fill buffer and write
      uint64_t limit = i + buffer.size();
      for (; i < count && i < limit; i++)
         buffer[i % buffer.size()] = factory(i);
      of.write(reinterpret_cast<char*>(buffer.data()), (buffer.size() - (limit - i)) * sizeof(T));
   }

   // Finish up
   of.flush();
   of.close();
   return of.good();
}

template<class T>
bool foreachInFileImpl(const std::string& fileName, std::function<void(T)> callback)
{
   // Open file
   std::ifstream in(fileName, std::ios::binary);
   if (!in.is_open() || !in.good())
      return false;

   // Loop over each entry
   T entry;
   while (true) {
      in.read(reinterpret_cast<char*>(&entry), sizeof(T));
      if (!in.good())
         break;
      callback(entry);
   }
   return true;
}

bool createTestFile(const std::string& fileName, uint64_t count, std::function<
      uint64_t(uint64_t)> factory)
{
   return createTestFileImpl<uint64_t>(fileName, count, factory);
}

bool foreachInFile(const std::string& fileName, std::function<void(uint64_t)> callback)
{
   return foreachInFileImpl<uint64_t>(fileName, callback);
}

}

#ifndef __Utility
#define __Utility

#include <functional>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

namespace dbi_utility {

/// Create a file with count entries created by the given factory function
template<class T>
bool createTestFile(const std::string& fileName, uint64_t count, std::function<T(int)> factory) {
  // Open file
  std::ofstream of(fileName, std::ios::binary);
  if(!of.is_open() || !of.good())
    return false;

  // Write file in buffered fashion
  const uint32_t kMaxBufferSize = 1<<22;
  std::vector<T> buffer(kMaxBufferSize / sizeof(T));
  for(uint64_t i=0; i<count;) {
    // Fill buffer and write
    uint64_t limit = i + buffer.size();
    for(;i<count && i<limit; i++)
      buffer[i%buffer.size()] = factory(i);
    of.write(reinterpret_cast<char*>(buffer.data()), (buffer.size()-(limit-i)) * sizeof(T));
  }
  
  // Finish up
  of.flush();
  of.close();
  return of.good();
}

/// Read a file and invoke callback function on every entry
template<class T>
bool readTestFile(const std::string& fileName, std::function<void(T)> callback) {
  // Open file
  std::ifstream in(fileName, std::ios::binary);
  if(!in.is_open() || !in.good())
    return false;

  // Loop over each entry
  T entry;
  while(true) {
    in.read(reinterpret_cast<char*>(&entry), sizeof(T));
    if(!in.good())
      break;
    callback(entry);
  }
}

}

#endif

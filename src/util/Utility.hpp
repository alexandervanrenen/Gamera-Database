#ifndef __Utility
#define __Utility

#include <fstream>
#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

namespace dbi {

namespace util {

/// Returns a random value
uint64_t ranny();

/// Create a file with count entries created by the given factory function
bool createTestFile(const std::string& fileName, uint64_t count, std::function<uint64_t(uint64_t)> factory);

/// Read a file and invoke callback function on every entry
bool foreachInFile(const std::string& fileName, std::function<void(uint64_t)> callback);

/// Create a file with random(not set) data of given size
bool createFile(const std::string& fileName, const uint64_t bytes);

/// Reads the length of the file
uint64_t getFileLength(const std::string& fileName);

/// Create a unique pointer
template<class T, class ... Arg>
std::unique_ptr<T> make_unique(Arg&& ...args)
{
   return std::unique_ptr<T>(new T(std::forward<Arg>(args)...));
}

/// Create random word
std::string randomWord(uint32_t min, uint32_t max);

}

}

#endif

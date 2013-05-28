#include "Utility.hpp"
#include <vector>
#include <fcntl.h>
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

namespace util {

uint64_t ranny()
{
   static uint64_t state = 88172645463325252ull;
   state ^= (state << 13);
   state ^= (state >> 7);
   return (state ^= (state << 17));
}

template<class T>
bool createTestFileImpl(const string& fileName, uint64_t count, function<T(int)> factory)
{
   // Open file
   ofstream of(fileName, ios::binary);
   if(!of.is_open() || !of.good())
      return false;

   // Write file in buffered fashion
   const uint32_t kMaxBufferSize = 1 << 22;
   vector<T> buffer(kMaxBufferSize / sizeof(uint64_t));
   for(uint64_t i = 0; i < count;) {
      // Fill buffer and write
      uint64_t limit = i + buffer.size();
      for(; i < count && i < limit; i++)
         buffer[i % buffer.size()] = factory(i);
      of.write(reinterpret_cast<char*>(buffer.data()), (buffer.size() - (limit - i)) * sizeof(uint64_t));
   }

   // Finish up
   of.flush();
   of.close();
   return of.good();
}

template<class T>
bool foreachInFileImpl(const string& fileName, function<void(T)> callback)
{
   // Open file
   ifstream in(fileName, ios::binary);
   if(!in.is_open() || !in.good())
      return false;

   // Loop over each entry
   T entry;
   while(true) {
      in.read(reinterpret_cast<char*>(&entry), sizeof(uint64_t));
      if(!in.good())
         break;
      callback(entry);
   }
   return true;
}

bool createTestFile(const string& fileName, uint64_t count, function<
uint64_t(uint64_t)> factory)
{
   return createTestFileImpl<uint64_t>(fileName, count, factory);
}

bool foreachInFile(const string& fileName, function<void(uint64_t)> callback)
{
   return foreachInFileImpl<uint64_t>(fileName, callback);
}

bool createFile(const string& fileName, const uint64_t bytes)
{
   if(system(("touch " + fileName).c_str()) == -1)
      return false;
   int fileFD = open(fileName.c_str(), O_RDWR);
   if(fcntl(fileFD, F_GETFD) == -1)
      return false;
   return posix_fallocate(fileFD, 0, bytes) == 0;
}

string randomWord(uint32_t min, uint32_t max)
{
   assert(max > min);
   uint32_t length = ranny()%(max-min) + min;
   string word(length, '_');
   for(uint32_t i = 0; i < length; i++)
      word[i] = 'a' + (ranny() % 26);
   return word;
}

}

}

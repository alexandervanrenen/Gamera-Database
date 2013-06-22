#include "Utility.hpp"
#include "Random.hpp"
#include <cassert>
#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

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
   ofstream out(fileName);
   if(!out.is_open() || !out.good())
       return false;
   vector<char> data(bytes);
   out.write(data.data(), bytes);
   out.flush();
   if(!out.good()) {
      remove(fileName.c_str());
      return false;
   }
   out.close();
   assert(getFileLength(fileName) == bytes);
   return true;
}

uint64_t getFileLength(const string& fileName)
{
   int fileFD = open(fileName.c_str(), O_RDWR);
   if(fcntl(fileFD, F_GETFL) == -1)
      return 0;
   struct stat st;
   fstat(fileFD, &st);
   close(fileFD);
   return st.st_size;
}

string loadFileToMemory(const string& fileName)
{
   uint64_t length = getFileLength(fileName);
   string data(length, 'a');
   ifstream in(fileName);
   in.read(&data[0], length);
   return move(data);
}

namespace {

uint64_t applyPrecision(uint64_t input, uint32_t precision)
{
    uint32_t digits = log10(input) + 1;
    if(digits <= precision)
        return input;
    uint32_t invalidDigits = pow(10, digits - precision);
    return (uint64_t)((double)input/invalidDigits+.5f)*invalidDigits;
}

}

string formatTime(chrono::nanoseconds ns, uint32_t precision)
{
   ostringstream os;

   uint64_t timeSpan = applyPrecision(ns.count(), precision);

   // Convert to right unit
   if(timeSpan < 1000ll)
      os << timeSpan << "ns";
   else if(timeSpan < 1000ll * 1000ll)
      os << timeSpan/1000.0f << "us";
   else if(timeSpan < 1000ll * 1000ll * 1000ll)
      os << timeSpan / 1000.0f / 1000.0f << "ms";
   else if(timeSpan < 60l * 1000ll * 1000ll * 1000ll)
      os << timeSpan / 1000.0f / 1000.0f / 1000.0f << "s";
   else if(timeSpan < 60l * 60l * 1000ll * 1000ll * 1000ll)
      os << timeSpan/1000.0f / 1000.0f / 1000.0f / 60.0f << "m";
   else
      os << timeSpan/1000.0f / 1000.0f / 1000.0f / 60.0f / 60.0f<< "h";

   return os.str();
}

string randomWord(util::Random& ranny, uint32_t min, uint32_t max)
{
   assert(max > min);
   uint32_t length = ranny.rand()%(max-min) + min;
   string word(length, '_');
   for(uint32_t i = 0; i < length; i++)
      word[i] = 'a' + (ranny.rand() % 26);
   return word;
}

}

}

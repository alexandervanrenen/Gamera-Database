#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <cassert>

namespace dbi {

class Record {
public:
   explicit Record(const char* const dataIn, unsigned length)
   : memory(length)
   {
      assert(length!=0);
      memcpy(memory.data(), dataIn, length);
   }
   explicit Record(const std::string& dataIn)
   : memory(dataIn.size())
   {
      assert(dataIn.size()!=0);
      memcpy(memory.data(), dataIn.data(), dataIn.size());
   }
   explicit Record(const std::vector<char>& dataIn)
   : memory(dataIn.size())
   {
      assert(dataIn.size()!=0);
      memcpy(memory.data(), dataIn.data(), dataIn.size());
   }
   Record(Record&& other)
   : memory(std::move(other.memory))
   {
   }
   const Record& operator= (Record&& other)
   {
      memory = std::move(other.memory);
      return *this;
   }

   bool operator==(const Record& other) const
   {
      return memory.size() == other.memory.size() && 0 == memcmp(memory.data(), other.memory.data(), memory.size());
   }
   bool operator!=(const Record& other) const
   {
      return !(*this == other);
   }
   bool operator<(const Record& other) const
   {
      return memory < other.memory;
   }

   const char* data() const
   {
      return memory.data();
   }

   uint32_t size() const
   {
      return memory.size();
   }

   Record& operator=(Record& rhs) = delete;
   Record(const Record& other) = delete;
private:
   std::vector<char> memory;
};

}

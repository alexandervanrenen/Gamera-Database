#pragma once

#include <string>
#include <cstring>
#include <vector>

namespace dbi {

class Record {
public:
   explicit Record(const char* const dataIn, unsigned length) : memory(length) {memcpy(memory.data(), dataIn, length);}
   explicit Record(const std::string& dataIn) : memory(dataIn.size()) {memcpy(memory.data(), dataIn.data(), dataIn.size());}
   explicit Record(const std::vector<char>& dataIn) : memory(dataIn.size()) {memcpy(memory.data(), dataIn.data(), dataIn.size());}
   Record(Record&& other) : memory(std::move(other.memory)) {}

   bool operator==(const Record& other) const {return memory.size()==other.memory.size() && 0==memcmp(memory.data(), other.memory.data(), memory.size());}
   bool operator!=(const Record& other) const {return !(*this==other);}

   const char* data() const {return memory.data();}

   uint32_t size() const {return memory.size();}

private:
   std::vector<char> memory;

   Record& operator=(Record& rhs) = delete;
   Record(Record& t) = delete;
};

}

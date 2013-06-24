#pragma once

#include <vector>
#include <cstdint>

namespace harriet { class Value; }

namespace dbi {

class IndexKeySchema;

class IndexKey {
public:
   IndexKey(std::vector<harriet::Value>&& values);
   IndexKey(std::vector<harriet::Value*> valueReferences);
   IndexKey(const IndexKey&) = delete;
   IndexKey& operator=(const IndexKey&) = delete;
   IndexKey(IndexKey&&);
   IndexKey& operator=(IndexKey&&);
   ~IndexKey();

   uint64_t bytes() const;

   void writeToMem(char* mem) const; // Caller has to make sure that there is at least size() byte of vaild memory
   static IndexKey readFromMemory(const char* ptr, const IndexKeySchema& schema); // Copies the memory

private:
   std::vector<harriet::Value> values;
   std::vector<harriet::Value*> valueReferences;
   friend class IndexKeyComparator;
};

}

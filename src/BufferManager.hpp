#ifndef __BufferManager
#define __BufferManager

#include "util/Utility.hpp"
#include <cassert>
#include <vector>

template<class T>
class BufferManager {
public:
   BufferManager(uint64_t size, uint64_t pageSize)
         : buffer(std::vector<char>(size)), pageSize(pageSize)
   {
   }

   char* begin()
   {
      return buffer.data();
   }

   char* end()
   {
      return buffer.data() + buffer.size();
   }

   std::unique_ptr<Page<T>> getPage(uint64_t id)
   {
      assert(id * pageSize < buffer.size());
      return dbiu::make_unique<Page<T>>(buffer.data() + (id * pageSize), pageSize);
   }

   uint64_t size() const
   {
      return buffer.size();
   }

private:
   std::vector<char> buffer;
   uint64_t pageSize;
};

#endif

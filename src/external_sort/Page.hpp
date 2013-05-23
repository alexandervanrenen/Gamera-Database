#ifndef __Page
#define __Page

#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <stdio.h>

namespace dbi {

struct Page {
   Page(char* memory, uint64_t pageSize)
   : memory(memory), pageSize(pageSize)
   {
      assert(pageSize % sizeof(uint64_t) == 0);
      assert(pageSize != 0);
   }

   uint64_t get(uint64_t id) const
   {
      assert(id * sizeof(uint64_t) < pageSize);
      return *reinterpret_cast<uint64_t*>(memory + (id * sizeof(uint64_t)));
   }

   void set(uint64_t id, uint64_t value)
   {
      assert(id * sizeof(uint64_t) < pageSize);
      *reinterpret_cast<uint64_t*>(memory + (id * sizeof(uint64_t))) = value;
   }

   char* begin()
   {
      return memory;
   }

   uint64_t size()
   {
      return pageSize;
   }

   uint64_t entryCount()
   {
      return pageSize / sizeof(uint64_t);
   }

private:
   char* memory;
   uint64_t pageSize;
};

}

#endif

#ifndef __Page
#define __Page

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <list>
#include <memory>

template<class T>
struct Page {
   Page(char* memory, uint64_t pageSize)
   : memory(memory), pageSize(pageSize)
   {
      assert(pageSize % sizeof(T) == 0);
      assert(pageSize != 0);
   }

   T get(uint64_t id) const
   {
      assert(id*sizeof(T) < pageSize);
      return *reinterpret_cast<T*>(memory + (id * sizeof(T)));
   }

   void set(uint64_t id, const T& value)
   {
      assert(id*sizeof(T) < pageSize);
      *reinterpret_cast<T*>(memory + (id * sizeof(T))) = value;
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
      return pageSize / sizeof(T);
   }

private:
   char* memory;
   uint64_t pageSize;
};

#endif

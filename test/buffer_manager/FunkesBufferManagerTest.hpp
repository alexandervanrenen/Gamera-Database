#pragma once

#include "buffer_manager/BufferManager.hpp"
#include "test/TestConfig.hpp"
#include <assert.h>
#include <iostream>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

dbi::BufferManager* bm;
unsigned pagesOnDisk;
unsigned pagesInRAM;
unsigned threadCount;
unsigned* threadSeed;
volatile bool stop = false;

unsigned randomPage(unsigned threadNum)
{
   // pseudo-gaussian, causes skewed access pattern
   unsigned page = 0;
   for(unsigned i = 0; i < 20; i++)
      page += rand_r(&threadSeed[threadNum]) % pagesOnDisk;
   return page / 20;
}

static void* scan(void * /*arg*/)
{
   // scan all pages and check if the counters are not decreasing
   unsigned counters[pagesOnDisk];
   for(unsigned i = 0; i < pagesOnDisk; i++)
      counters[i] = 0;

   while(!stop) {
      unsigned start = random() % (pagesOnDisk - 10);
      for(unsigned page = start; page < start + 10; page++) {
         dbi::BufferFrame& bf = bm->fixPage(dbi::PageId(page), false);
         unsigned newcount = reinterpret_cast<unsigned*>(bf.data())[0];
         assert(counters[page] <= newcount);
         counters[page] = newcount;
         bm->unfixPage(bf, false);
      }
   }

   return NULL;
}

static void* readWrite(void *arg)
{
   // read or write random pages
   uintptr_t threadNum = reinterpret_cast<uintptr_t>(arg);

   uintptr_t count = 0;
   for(unsigned i = 0; i < 1000 / threadCount; i++) {
      bool isWrite = rand_r(&threadSeed[threadNum]) % 128 < 10;
      dbi::BufferFrame& bf = bm->fixPage(dbi::PageId(randomPage(threadNum)), isWrite);

      if(isWrite) {
         count++;
         reinterpret_cast<unsigned*>(bf.data())[0]++;
      }
      bm->unfixPage(bf, isWrite);
   }

   return reinterpret_cast<void*>(count);
}

int main_funke(int argc, char** argv)
{
   if(argc == 5) {
      pagesOnDisk = atoi(argv[2]);
      pagesInRAM = atoi(argv[3]);
      threadCount = atoi(argv[4]);
   } else {
      std::cerr << "usage: " << argv[0] << " <file> <pagesOnDisk> <pagesInRAM> <threads>" << std::endl;
      exit(1);
   }

   threadSeed = new unsigned[threadCount];
   for(unsigned i = 0; i < threadCount; i++)
      threadSeed[i] = i * 97134;

   bm = new dbi::BufferManager(argv[1], pagesInRAM);

   pthread_t threads[threadCount];
   pthread_attr_t pattr;
   pthread_attr_init(&pattr);

   // set all counters to 0
   for(unsigned i = 0; i < pagesOnDisk; i++) {
      dbi::BufferFrame& bf = bm->fixPage(dbi::PageId(i), true);
      reinterpret_cast<unsigned*>(bf.data())[0] = 0;
      bm->unfixPage(bf, true);
   }

   // start scan thread
   pthread_t scanThread;
   pthread_create(&scanThread, &pattr, scan, NULL);

   // start read/write threads
   for(unsigned i = 0; i < threadCount; i++)
      pthread_create(&threads[i], &pattr, readWrite, reinterpret_cast<void*>(i));

   // wait for read/write threads
   unsigned totalCount = 0;
   for(unsigned i = 0; i < threadCount; i++) {
      void *ret;
      pthread_join(threads[i], &ret);
      totalCount += reinterpret_cast<uintptr_t>(ret);
   }

   // wait for scan thread
   stop = true;
   pthread_join(scanThread, NULL);

   // restart buffer manager
   delete bm;
   bm = new dbi::BufferManager(argv[1], pagesInRAM);

   // check counter
   unsigned totalCountOnDisk = 0;
   for(unsigned i = 0; i < pagesOnDisk; i++) {
      dbi::BufferFrame& bf = bm->fixPage(dbi::PageId(i), false);
      totalCountOnDisk += reinterpret_cast<unsigned*>(bf.data())[0];
      bm->unfixPage(bf, false);
   }
   if(totalCount == totalCountOnDisk) {
      delete[] threadSeed;
      delete bm;
      return 0;
   } else {
      std::cerr << "error: expected " << totalCount << " but got " << totalCountOnDisk << std::endl;
      delete[] threadSeed;
      delete bm;
      return 1;
   }
}

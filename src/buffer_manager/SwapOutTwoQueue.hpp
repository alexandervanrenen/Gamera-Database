#pragma once

#include "util/ConcurrentOffsetHash.hpp"
#include <iostream>
#include <list>
#include <mutex>

namespace dbi {

class BufferFrame;

class SwapOutTwoQueue {
public:

   void initialize(util::ConcurrentOffsetHash<PageId, BufferFrame>& bufferFrameDir)
   {
      // Add all pages to the fifo queue
      for(auto& iter : bufferFrameDir.data()) {
         iter.value.listIterator = fifo.insert(fifo.begin(), &iter.value);
         iter.value.isInFifoQueue = true;
      }
   }

   BufferFrame& findPageToSwapOut(util::ConcurrentOffsetHash<PageId, BufferFrame>&)
   {
      std::unique_lock<std::mutex> l(guard);
      while(true) {
         // Loop over all entries in fifo queue and use the first one which is not locked
         for(auto iter = fifo.begin(); iter != fifo.end(); iter++) {
            if((*iter)->accessGuard.tryLockForWriting()) {
               BufferFrame* result = *iter;
               fifo.erase(iter);
               fifo.insert(fifo.end(), result);
               return *result;
            }
         }

         // Loop over all entries in lru queue and use the first one which is not locked
         for(auto iter = lru.begin(); iter != lru.end(); iter++) {
            if((*iter)->accessGuard.tryLockForWriting()) {
               BufferFrame* result = *iter;
               lru.erase(iter);
               lru.insert(lru.end(), result);
               return *result;
            }
         }
      }
   }

   void onFixPage(BufferFrame& bufferFrame)
   {
      std::unique_lock<std::mutex> l(guard);
      if(bufferFrame.isInFifoQueue) {
         fifo.erase(bufferFrame.listIterator);
         bufferFrame.listIterator = lru.insert(fifo.end(), &bufferFrame);
      } else {
         bufferFrame.isInFifoQueue = false;
         lru.erase(bufferFrame.listIterator);
         bufferFrame.listIterator = lru.insert(fifo.end(), &bufferFrame);
      }
   }

   void onUnfixPage(BufferFrame&)
   {
   }

private:
   // Use simple locking for now. We are not planing to use this strategy.
   // We will use the second chance algorithm, which does not require any locking.
   // But our design will allow to exchange the algorithms as we see fit.
   std::mutex guard;

   std::list<BufferFrame*> fifo;
   std::list<BufferFrame*> lru;
};

}

#pragma once

#include "util/ConcurrentOffsetHash.hpp"
#include <list>
#include <mutex>
#include <iostream>

namespace dbi {

class BufferFrame;

class SwapOutTwoQueue {
public:

   void initialize(util::ConcurrentOffsetHash<PageId, BufferFrame>& bufferFrameDir)
   {
      for(auto& iter : bufferFrameDir.data()) {
         iter.value.listIterator = fifo.insert(fifo.begin(), &iter.value);
         iter.value.isInFifoQueue = true;
      }
   }

   BufferFrame& findPageToSwapOut(util::ConcurrentOffsetHash<PageId, BufferFrame>&)
   {
      std::unique_lock<std::mutex> l(guard);
      while(true) {
         // Get page from queue
         BufferFrame* result;
         if(!fifo.empty()) { // TODO: May loop for a while, fifo.size() == 1 is not good ;)
            result = fifo.front();
            fifo.pop_front();
         } else {
            result = lru.front();
            lru.pop_front();
         }

         // Otherwise: put back into queue and try again
         result->isInFifoQueue = true;
         result->listIterator = fifo.insert(fifo.end(), result);

         // Try to lock (should always work ..)
         if(result->accessGuard.tryLockForWriting())
            return *result;
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

   void onUnfixPage(BufferFrame&) {}

private:
   // Use simple locking for now. We are not planing to use this strategy.
   // We will use the second chance algorithm, which does not require any locking.
   // But our design will allow to exchange the algorithms as we see fit.
   std::mutex guard;

   std::list<BufferFrame*> fifo;
   std::list<BufferFrame*> lru;
};

}

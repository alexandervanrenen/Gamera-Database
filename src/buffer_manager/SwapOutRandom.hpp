#pragma once

#include "util/ConcurrentOffsetHash.hpp"
#include "util/Random.hpp"

namespace dbi {

class BufferFrame;

class SwapOutRandom {
public:
   void initialize(util::ConcurrentOffsetHash<PageId, BufferFrame>&)
   {
   }

   BufferFrame& findPageToSwapOut(util::ConcurrentOffsetHash<PageId, BufferFrame>& bufferFrameDir)
   {
      while(true) {
         BufferFrame& result = bufferFrameDir.data()[ranny.rand() % bufferFrameDir.data().size()].value;
         if(result.accessGuard.tryLockForWriting())
            return result;
      }
   }

   void onUnfixPage(BufferFrame&)
   {
   }
   void onFixPage(BufferFrame&)
   {
   }

private:
   util::Random ranny;
};

}

#pragma once

#include "util/ConcurrentOffsetHash.hpp"
#include "util/Utility.hpp"

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
         BufferFrame& result = bufferFrameDir.data()[util::ranny() % bufferFrameDir.data().size()].value;
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
};

}

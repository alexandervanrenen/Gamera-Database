#pragma once

#include "util/ConcurrentOffsetHash.hpp"

namespace dbi {

class BufferFrame;

class SwapOutRandom {
public:
   void initialize(util::ConcurrentOffsetHash<PageId, BufferFrame>&) {}

   BufferFrame& findPageToSwapOut(util::ConcurrentOffsetHash<PageId, BufferFrame>& bufferFrameDir)
   {
      while(true) {
         BufferFrame& result = bufferFrameDir.data()[random()%bufferFrameDir.data().size()].value;
         if(result.accessGuard.tryLockForWriting())
            return result;
      }
   }

   void onUnfixPage(BufferFrame&) {}
   void onFixPage(BufferFrame&) {}
};

}

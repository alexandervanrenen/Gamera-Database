#pragma once

#include "util/ConcurrentOffsetHash.hpp"

namespace dbi {

class BufferFrame;

class SwapOutSecondChance {
public:
   void initialize(util::ConcurrentOffsetHash<PageId, BufferFrame>&) {}

   BufferFrame& findPageToSwapOut(util::ConcurrentOffsetHash<PageId, BufferFrame>& bufferFrameDir)
   {
      while(true) {
         auto& bufferFrame = bufferFrameDir.data()[index].value;
         index = (index+1) % bufferFrameDir.data().size();
         if(!bufferFrame.hasSecondChance && bufferFrame.accessGuard.tryLockForWriting())
            return bufferFrame; else
            bufferFrame.hasSecondChance = false;
      }
   }

   void onUnfixPage(BufferFrame& bufferFrame)
   {
      bufferFrame.hasSecondChance = true;
   }

   void onFixPage(BufferFrame&) {}

private:
   uint32_t index = 0; // Remember position in data set
};

}

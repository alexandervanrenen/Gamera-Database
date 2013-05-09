#pragma once

#include "util/ConcurrentOffsetHash.hpp"

namespace dbi {

class BufferFrame;

class SwapOutSecondChance {
public:
   BufferFrame& findPageToSwapOut(util::ConcurrentOffsetHash<PageId, BufferFrame>& bufferFrameDir)
   {
      while(true) {
         auto& bufferFrame = bufferFrameDir.data()[index].value;
         index = (index+1) % bufferFrameDir.data().size();
         if(bufferFrame.hasSecondChance)
            bufferFrame.hasSecondChance = false; else
            return bufferFrame;
      }
   }

   void onUnfixPage(BufferFrame& bufferFrame)
   {
      bufferFrame.hasSecondChance = true;
   }

private:
   uint32_t index = 0; // Remember position in data set
};

}

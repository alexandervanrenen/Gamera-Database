#pragma once

#include "util/ConcurrentOffsetHash.hpp"

namespace dbi {

class BufferFrame;

class SwapOutRandom {
public:
   BufferFrame& findPageToSwapOut(util::ConcurrentOffsetHash<PageId, BufferFrame>& bufferFrameDir)
   {
      return bufferFrameDir.data()[random()%bufferFrameDir.data().size()].value;
   }

   void onUnfixPage(BufferFrame&) {}

};

}

#pragma once

#include "common/Config.hpp"
#include "util/ReadWriteLock.hpp"
#include <array>
#include <condition_variable>
#include <cstdint>
#include <list>

namespace dbi {

// TODO: move algorithm specific data out of here
class BufferFrame {
public:
   /// Access page memory, is mapped to disc by the buffer manager
   char* data();

private:
   std::array<char, kPageSize> pageContent;
   bool isDirty = false;
   PageId pageId = kInvalidPageId;
   using BufferFrameLockType = util::ReadWriteLock;
   BufferFrameLockType accessGuard;

   // Second chance algorithm specific data
   bool hasSecondChance = true;

   // Position of this frame in queues of the two queue algorithm
   std::list<BufferFrame*>::iterator listIterator;
   bool isInFifoQueue;

   friend class BufferManager;
   friend class SwapOutSecondChance;
   friend class SwapOutTwoQueue;
   friend class SwapOutRandom;
};

}

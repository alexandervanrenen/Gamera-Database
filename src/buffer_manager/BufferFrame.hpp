#pragma once

#include "common/Config.hpp"
#include "util/ReadWriteSpinLock.hpp"
#include <array>
#include <list>
#include <cstdint>
#include <condition_variable>

namespace dbi {

// TODO: move algorithm specific data out of here
class BufferFrame {
public:
    /// Access page memory, is mapped to disc by the buffer manager
    char* getData();

private:
    std::array<char, kPageSize> data;
    bool isDirty = false;
    PageId pageId = 0;
    using BufferFrameLockType = util::ReadWriteSpinLock;
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

#pragma once

#include "common/Config.hpp"
#include "util/ReadWriteLock.hpp"
#include <array>
#include <cstdint>
#include <condition_variable>

namespace dbi {

class BufferFrame {
public:
    /// Access page memory, is mapped to disc by the buffer manager
    char* getData();

private:
    std::array<char, kPageSize> data;
    bool isDirty = false;
    PageId pageId = 0;
    util::ReadWriteLock accessGuard;

    friend class BufferManager;
};

}

#ifndef BufferFrame_hpp
#define BufferFrame_hpp

#include "common/Config.hpp"
#include "util/ReadWriteLock.hpp"
#include <array>
#include <cstdint>
#include <condition_variable>

namespace dbi {

class BufferFrame {
public:
    /// A buffer frame should offer a method giving access to the buffered page. Except
    /// for the buffered page, BufferFrame objects can also store control information
    /// (page ID, dirtyness, . . . ).
    char* getData();

private:
    std::array<char, kPageSize> data;
    bool isDirty = false;
    PageId pageId = 0;
    util::ReadWriteLock accessGuard;

    friend class BufferManager;
};

}

#endif

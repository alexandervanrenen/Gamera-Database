#ifndef BufferFrame_hpp
#define BufferFrame_hpp

#include "common/Config.hpp"
#include <array>
#include <cstdint>
#include <condition_variable>

namespace dbi {

class BufferFrame {
public:
    /// A buffer frame should offer a method giving access to the buffered page. Except
    /// for the buffered page, BufferFrame objects can also store control information
    /// (page ID, dirtyness, . . . ).
    void* getData();

private:
    std::array<char, kPageSize> data;
    bool exclusive = false;
    bool isDirty = false;
    uint32_t refCount = 0;
    uint32_t pageId = 0;
    uint32_t threadsWaiting = 0;
    std::condition_variable cond;

    friend class BufferManager;
};

}

#endif

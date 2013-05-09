#pragma once

#include "common/Config.hpp"
#include "BufferFrame.hpp"
#include "util/ConcurrentOffsetHash.hpp"
#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace dbi {

namespace util {
    template<bool>
    class StatisticsCollector;
}

class BufferManager {
public:
    /// Create a new instance that manages size frames and operates on the file filename.
    /// memoryPages is the number of pages in memory (not disc)
    /// The complete number of pages (on disc are given by the file size)
    BufferManager(const std::string& filename, uint64_t memoryPagesCount);

    /// A method to retrieve frames given a page ID and indicating whether the page will be
    /// held exclusively by this thread or not. The method can fail if no free frame is
    /// available and no used frame can be freed.
    BufferFrame& fixPage(PageId pageId, bool exclusive);  

    /// Return a frame to the buffer manager indicating whether it is dirty or not. If
    /// dirty, the page manager must write it back to disk. It does not have to write it
    /// back immediately, but must not write it back before unfixPage is called.
    void unfixPage(BufferFrame& frame, bool isDirty);

    /// Access maximum number of pages
    uint64_t getNumMemoryPages() {return memoryPagesCount;}
    uint64_t getNumDiscPages() {return discPagesCount;}

    // Write all data to disc
    void flush();

    /// Destructor. Write all dirty frames to disk and free all resources.
    ~BufferManager();

private:
    uint64_t memoryPagesCount;
    uint64_t discPagesCount;
    std::fstream file;

    std::mutex guard;

    void loadFrame(PageId pageId, BufferFrame& frame);
    void saveFrame(BufferFrame& frame);

    ///points from a page id to the containing buffer frame within the memory
    util::ConcurrentOffsetHash<PageId, BufferFrame> bufferFrameDir;
    ///prevents threads from trying to load a page from disc at the same time
    std::mutex pageLoadGuard;

    ///Tries to lock a buffer frame in which the provided pageId is expected 
    ///For the purpose of concurrency protection, the pageId of the buffer frame is compared with the provided one after the lock has been aquired.
    ///In case the pageId does not match, the lock is released and fixPage is called instead.
    ///Otherwise the locked buffer frame is returned.
    BufferFrame& tryLockBufferFrame(BufferFrame& bufferFrame, const PageId expectedPageId, const bool exclusive);

    std::unique_ptr<util::StatisticsCollector<false>> stats;
};

const static bool kExclusive = true;
const static bool kShared = false;
const static bool kClean = false;
const static bool kDirty = true;

}

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
    /// Constructor: The complete number of pages (on disc are given by the file size)
    BufferManager(const std::string& filename, uint64_t memoryPagesCount);

    /// A method to retrieve frames given a page ID and indicating whether the page will be held exclusively by this thread or not.
    BufferFrame& fixPage(PageId pageId, bool exclusive);  

    /// Return a frame to the buffer manager indicating whether it is dirty or not.
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

    /// Helper
    void loadFrame(PageId pageId, BufferFrame& frame);
    void saveFrame(BufferFrame& frame);

    /// Points from a page id to the buffer frame containing this page
    util::ConcurrentOffsetHash<PageId, BufferFrame> bufferFrameDir;
    /// Prevents threads from trying to load a page from disc at the same time
    std::mutex pageLoadGuard;

    /// Tries to lock a buffer frame in which the provided pageId is expected
    /// For the purpose of concurrency protection, the pageId of the buffer frame is compared with the provided one after the lock has been aquired.
    /// In case the pageId does not match, the lock is released and fixPage is called instead.
    /// Otherwise the locked buffer frame is returned.
    BufferFrame& tryLockBufferFrame(BufferFrame& bufferFrame, const PageId expectedPageId, const bool exclusive);

    /// Collecting performance
    std::unique_ptr<util::StatisticsCollector<false>> stats;
};

const static bool kExclusive = true;
const static bool kShared = false;
const static bool kClean = false;
const static bool kDirty = true;

}

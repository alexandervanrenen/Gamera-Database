#ifndef _BufferManager_hpp
#define _BufferManager_hpp

#include "common/Config.hpp"
#include "BufferFrame.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include "util/ConcurrentOffsetHash.hpp"

namespace dbi {

class BufferManager {
public:
    /// Create a new instance that manages size frames and operates on the file filename.
    /// memoryPages is the number of pages in memory (not disc)
    /// The complete number of pages (on disc are given by the file size)
    BufferManager(const std::string& filename, uint64_t memoryPages);

    /// A method to retrieve frames given a page ID and indicating whether the page will be
    /// held exclusively by this thread or not. The method can fail if no free frame is
    /// available and no used frame can be freed.
    BufferFrame& fixPage(PageId pageId, bool exclusive);  
    
    /// new fixPage implementation according to activity diagram. Semantics are equal to fixPage(PageId pageId, bool exclusive).
    BufferFrame& fixPage2(PageId pageId, bool exclusive);

    /// Return a frame to the buffer manager indicating whether it is dirty or not. If
    /// dirty, the page manager must write it back to disk. It does not have to write it
    /// back immediately, but must not write it back before unfixPage is called.
    void unfixPage(BufferFrame& frame, bool isDirty);
    
    ///new unfixPage implenmentation according to activity diagram. Semantics are equal to unfixPage(BufferFrame& frame, bool isDirty).
    void unfixPage2(BufferFrame& frame, bool isDirty);

    /// Access maximum number of pages
    uint64_t getNumMemoryPages() {return memoryPages;}
    uint64_t getNumDiscPages() {return discPages;}

    // Write all data to disc
    void flush();

    /// Destructor. Write all dirty frames to disk and free all resources.
    ~BufferManager();

private:
    uint64_t memoryPages;
    uint64_t discPages;
    std::fstream file;

    std::vector<std::unique_ptr<BufferFrame>> allFrames;
    std::unordered_map<PageId, BufferFrame*> loadedFrames;
    std::unordered_map<PageId, BufferFrame*> unusedFrames;
    std::vector<BufferFrame*> freeFrames;

    std::mutex guard;
    std::condition_variable cond;

    void loadFrame(PageId pageId, BufferFrame& frame);
    void saveFrame(BufferFrame& frame);
    
    ///new fixPage2 required members
    ///points from a page id to the containing buffer frame within the memory
    util::ConcurrentOffsetHash<PageId, BufferFrame> bufferFrameDir;
    ///prevents threads from trying to load a page from disc at the same time
    std::mutex pageLoadGuard;
    
    ///Tries to lock a buffer frame in which the provided pageId is expected 
    ///For the purpose of concurrency protection, the pageId of the buffer frame is compared with the provided one after the lock has been aquired.
    ///In case the pageId does not match, the lock is released and fixPage is called instead.
    ///Otherwise the locked buffer frame is returned.
    BufferFrame& tryLockBufferFrame(BufferFrame& bufferFrame, const PageId expectedPageId, const bool exclusive);
    
    //TODO: add method to get buffer frame to be replaced    
};

const static bool kExclusive = true;
const static bool kShared = false;
const static bool kClean = false;
const static bool kDirty = true;

}

#endif

#ifndef _BufferManager_hpp
#define _BufferManager_hpp

#include "BufferFrame.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <condition_variable>

namespace dbi {

class BufferManager {
public:
    /// Create a new instance that manages size frames and operates on the file filename.
    /// Size is the number of pages
    BufferManager(const std::string& filename, uint64_t size);

    /// A method to retrieve frames given a page ID and indicating whether the page will be
    /// held exclusively by this thread or not. The method can fail if no free frame is
    /// available and no used frame can be freed.
    BufferFrame& fixPage(unsigned pageId, bool exclusive);

    /// Return a frame to the buffer manager indicating whether it is dirty or not. If
    /// dirty, the page manager must write it back to disk. It does not have to write it
    /// back immediately, but must not write it back before unfixPage is called.
    void unfixPage(BufferFrame& frame, bool isDirty);

    /// Access maximum number of pages
    uint64_t getNumPages() {return size; }

    // Write all data to disc
    void flush();

    /// Destructor. Write all dirty frames to disk and free all resources.
    ~BufferManager();

private:
    uint64_t size;
    std::fstream file;

    std::vector<std::unique_ptr<BufferFrame>> allFrames;
    std::unordered_map<unsigned, BufferFrame*> loadedFrames;
    std::unordered_map<unsigned, BufferFrame*> unusedFrames;
    std::vector<BufferFrame*> freeFrames;

    std::mutex guard;
    std::condition_variable cond;

    void loadFrame(unsigned pageId, BufferFrame& frame);
    void saveFrame(BufferFrame& frame);
};

}

#endif

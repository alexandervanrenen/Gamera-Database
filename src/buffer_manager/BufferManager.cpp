#include "BufferManager.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include <fstream>
#include <cassert>

namespace dbi {

using namespace std;

BufferManager::BufferManager(const std::string& fileName, uint64_t size)
: size(size)
, file(fileName.c_str(), ios::out | ios::in)
, allFrames(size)
{
    // Check length of the file
    assert(file.is_open() && file.good());
    file.seekg(0, ios::end);
    size_t fileLength = file.tellg();
    file.seekg(0, ios::beg);
    assert(fileLength > 0 && fileLength%kPageSize==0);

    // Allocate management data structures
    for(auto& iter : allFrames) {
        iter = dbiu::make_unique<BufferFrame>();
        freeFrames.push_back(iter.get());
    }
}

BufferFrame& BufferManager::fixPage(unsigned pageId, bool exclusive)
{
    unique_lock<mutex> l(guard);

    // Case 1: Page is not loaded
    auto iter = loadedFrames.find(pageId);
    if(iter == loadedFrames.end()) {
        /// Wait if no page is free
        cond.wait(l, [&](){ return !freeFrames.empty() || !unusedFrames.empty(); });

        /// Swap out
        if(freeFrames.empty()) {
            BufferFrame* framePtr = unusedFrames.begin()->second;
            saveFrame(*framePtr);
            loadedFrames.erase(pageId);
            unusedFrames.erase(pageId);
            freeFrames.push_back(framePtr);
        }
        assert(!freeFrames.empty());

        // Get available frame
        BufferFrame& frame = *freeFrames.back();
        freeFrames.pop_back();
        loadFrame(pageId, frame);
        assert(frame.refCount == 0);
        frame.refCount++;
        frame.exclusive = exclusive;
        return frame;
    }

    // Case 2: Page is loaded and in any way reusable
    if((!iter->second->exclusive && !exclusive) || iter->second->refCount==0) {
        iter->second->exclusive = exclusive;
        if(iter->second->refCount==0)
            unusedFrames.erase(pageId);
        iter->second->refCount++;
        return *iter->second;
    }

    // Case 3: Otherwise
    iter->second->threadsWaiting++;
    while(iter->second->refCount != 0)
        iter->second->cond.wait(l);
    iter->second->refCount++;
    iter->second->exclusive = exclusive;
    iter->second->threadsWaiting--;
    return *iter->second;
}

void BufferManager::unfixPage(BufferFrame& frame, bool isDirty)
{
    unique_lock<mutex> l(guard);
    assert(frame.refCount > 0);

    frame.isDirty |= isDirty;
    frame.refCount--;
    if(frame.refCount == 0 && frame.threadsWaiting == 0) {
        unusedFrames[frame.pageId] = &frame;
        cond.notify_one();
    } else {
        frame.cond.notify_one();
    }
}

void BufferManager::flush()
{
    unique_lock<mutex> l(guard);
    for(auto iter : loadedFrames)
        if(iter.second->isDirty)
            saveFrame(*iter.second);
}

BufferManager::~BufferManager()
{
    flush();
}

void BufferManager::loadFrame(unsigned pageId, BufferFrame& frame)
{
    assert(frame.refCount==0 && !frame.isDirty);
    file.seekg(pageId*kPageSize, ios::beg);
    file.read(frame.data.data(), kPageSize);
    frame.pageId = pageId;
    loadedFrames[pageId] = &frame;
}

void BufferManager::saveFrame(BufferFrame& frame)
{
    assert(frame.refCount==0);
    if(frame.isDirty) {
        file.seekg(frame.pageId*kPageSize, ios::beg);
        file.write(frame.data.data(), kPageSize);
    }
    frame.isDirty = false;
}

}

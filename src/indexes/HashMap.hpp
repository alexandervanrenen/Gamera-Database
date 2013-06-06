#pragma once

#include "HashMapSegment.hpp"
#include "common/Config.hpp"
#include "HashMapMetaPage.hpp"
#include "HashMapBucketPage.hpp"
#include "HashMapDirectoryPage.hpp"
#include "HashMapBucketPage.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "buffer_manager/BufferFrame.hpp"
#include <vector>
#include <cmath>
#include <utility>
#include <bitset>
#include <memory>

namespace dbi {

template<typename Key, typename Value>
class HashMap {
public:
    HashMap(HashMapSegment& segment);

    void insert(const Key& key, const Value& value);
    Value get(const Key& key);
    void remove(const Key& key);

    virtual void initializeExtent(const Extent& extent);

    const Extent grow();
    const Extent grow(uint64_t numPages);

    void dump();

private:
    HashMapSegment& segment;

    std::pair<PageId, uint32_t> mapToDirectory(const HashMapMetaPage& meta, const Key& key);
};

template<class Key, class Value>
HashMap<Key, Value>::HashMap(HashMapSegment& segment)
: segment(segment)
{

}

template<class Key, class Value>
void HashMap<Key, Value>::insert(const Key& key, const Value& value)
{
    // Get position
    auto& metaBf = segment.getMetaBufferFrame();
    auto& metaPage = reinterpret_cast<HashMapMetaPage&>(*metaBf.data());
    std::pair<PageId, uint32_t> position = mapToDirectory(metaPage, key);

    // Get directory
    auto& directoryBf = segment.fixGlobalPage(position.first, kShared);
    auto& directoryPage = reinterpret_cast<DirectoryPage&>(*directoryBf.data());
    PageId bucketPageId = directoryPage[position.second];
    segment.unfixPage(directoryBf, kClean);

    // Get bucket
    auto& bucketBf = segment.fixGlobalPage(bucketPageId, kExclusive);
    auto& bucketPage = reinterpret_cast<HashMapBucketPage&>(*bucketBf.data());
    bucketPage.insert(key, value);
    segment.unfixPage(bucketBf, kDirty);

    // // Update meta data
    metaPage.entries++;
    segment.unfixPage(metaBf, kDirty);
}

template<class Key, class Value>
Value HashMap<Key, Value>::get(const Key& key) {
    // uint16_t dirIndex = computeDirectoryIndex(key);
    // return directory[dirIndex]->get(key);
    throw;
}

template<class Key, class Value>
void HashMap<Key, Value>::remove(const Key& key) {
    throw;
}

template<class Key, class Value>
void HashMap<Key, Value>::initializeExtent(const Extent& extent)
{

}

template<class Key, class Value>
const Extent HashMap<Key, Value>::grow()
{
   Extent extent = Segment::grow();
   initializeExtent(extent);
   return extent;
}

template<class Key, class Value>
const Extent HashMap<Key, Value>::grow(uint64_t numPages)
{
   Extent extent = Segment::grow(numPages);
   initializeExtent(extent);
   return extent;
}

template<class Key, class Value>
void HashMap<Key, Value>::dump(std::ostream& os)
{
    // Get meta page
    auto& metaBf = segment.getMetaBufferFrame();
    auto& metaPage = reinterpret_cast<HashMapMetaPage&>(*metaBf.data());

    // Loop over each directory entry
    for(uint32_t i=0; i<(1<<metaPage.numRelevantBits); i++) {
        // Get the directory page
        PageId directoryPageId = metaPage.getDirectoryPage(i);
        auto& directoryBf = segment.fixGlobalPage(directoryPageId, kShared);
        auto& directoryPage = reinterpret_cast<DirectoryPage&>(*directoryBf.data());

        // Get the bucket and dump it
        PageId bucketPageId = directoryPage[position.second];
        segment.unfixPage(directoryBf, kClean);


    }
}

template<class Key, class Value>
std::pair<PageId, uint32_t> HashMap<Key, Value>::mapToDirectory(const HashMapMetaPage& meta, const Key& key)
{
    // Obtain position
    uint32_t h = std::hash<Key>()(key);
    uint32_t smallMask = (1<<(meta.numRelevantBits-1))-1;
    uint32_t bigMask = (1<<(meta.numRelevantBits))-1;
    uint32_t pos = h & smallMask;
    if(pos > meta.next)
        pos = h & bigMask;

    // Map to a concrete page and an offset
    PageId directoryPageId = meta.getDirectoryPage(pos / (kPageSize/sizeof(PageId)));
    uint32_t offset = pos % (kPageSize/sizeof(PageId));
    return std::make_pair(directoryPageId, offset);
}

}


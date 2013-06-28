#ifndef BTINNERNODE_HPP
#define BTINNERNODE_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <iostream>

#include "btree/BTNode.hpp"
#include "btree/BTreeBase.hpp"
#include "btree/CharIterator.hpp"

namespace dbi {

template <typename Key, typename C>
class BTInnerNode : public BTNode<Key, C> {
public:
    typedef CharIterator<PageId> Iterator;
    const static uint64_t headersize = 3 * sizeof(PageId) + 2 * sizeof(uint64_t);
    const static uint64_t datasize = kPageSize - headersize;
    const static uint64_t valuesize = sizeof(PageId);
    typedef std::array<char, datasize> CharArray;
    uint64_t nextindex = 0;
    PageId rightpointer;
    CharArray data;

    Iterator begin(uint64_t keysize) const {
        return Iterator(data.data(), keysize, nextindex); 
    }

    Iterator end(uint64_t keysize) const {
        return Iterator(data.data(), keysize, nextindex, nextindex+1);
    }
    
    Iterator last(uint64_t keysize) const {
        return Iterator(data.data(), keysize, nextindex, nextindex);
    }

    char* pointer() const {
        return data;
    }
    
    void put(const Key& k, const PageId& value, uint64_t index) {
        uint64_t off = index*(valuesize+k.size());
        k.writeToMem(data.data()+off);
        *((PageId*)(data.data()+off+k.size())) = value;
    }
    
    void put(const Key& k, const PageId& value, Iterator& it) {
        char* p = it.pointer();
        k.writeToMem(p);
        *((PageId*)(p+k.size())) = value;
    }


};


}


#endif

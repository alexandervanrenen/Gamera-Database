#ifndef BTLEAFNODE_HPP
#define BTLEAFNODE_HPP

#include <array>
#include <cstdint>
#include <iostream>

#include "btree/BTNode.hpp"
#include "btree/BTreeBase.hpp"
#include "btree/CharIterator.hpp"

namespace dbi {

template <typename Key, typename C>
class BTLeafNode : public BTNode<Key,C> {
public:
    typedef CharIterator<TupleId> Iterator;
    const static uint64_t headersize = 2* sizeof(uint64_t) + 3 * sizeof(PageId);
    const static uint64_t datasize = PAGESIZE - headersize;
    const static uint64_t valuesize = sizeof(TupleId);
    typedef std::array<char, datasize> CharArray;
    PageId nextpage = PageId(0); // Pointer to next leaf page (to iterate) (0 signals there is no next page)
    uint64_t nextindex = 0; // index of array to be used next (equals size of array if array is full)
    CharArray data;
    
    Iterator begin(uint64_t keysize) const {
        return Iterator(data.data(), keysize, nextindex); 
    }

    Iterator end(uint64_t keysize) const {
        return Iterator(data.data(), keysize, nextindex, nextindex);
    }
    
    Iterator last(uint64_t keysize) const {
        return Iterator(data.data(), keysize, nextindex, nextindex-1);
    }

    char* pointer() const {
        return data.data();
    }

    void put(const Key& k, const TupleId& value, uint64_t index) {
        uint64_t off = index*(valuesize+k.bytes());
        k.writeToMem(data.data()+off);
        *((TupleId*)(data.data()+off+k.bytes())) = value;
    }

    void put(const Key& k, const TupleId& value, char* p) {
        k.writeToMem(p);
        *((TupleId*)(p+k.bytes())) = value;
    }
    
    void put(const Key& k, const TupleId& value, Iterator& it) {
        char* p = it.pointer();
        k.writeToMem(p);
        *((TupleId*)(p+k.bytes())) = value;
    }

};


}


#endif

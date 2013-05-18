#ifndef BTLEAFNODE_HPP
#define BTLEAFNODE_HPP

#include <cstdint>
#include <array>
#include <iostream>

#include "btree/BTreeBase.hpp"
#include "btree/BTNode.hpp"

namespace dbi {

template <typename Key, typename C>
class BTLeafNode : public BTNode<Key,C> {
public:
    typedef std::array<std::pair<Key, TID>, (PAGESIZE - sizeof(uint16_t) - 3 * sizeof(PageId)) / (sizeof(Key)+sizeof(TID))> Values;
    const static uint64_t numkeys = (PAGESIZE - sizeof(uint16_t) - 3 * sizeof(PageId)) / (sizeof(Key)+sizeof(TID));
    PageId nextpage = 0; // Pointer to next leaf page (to iterate) (0 signals there is no next page)
    uint64_t nextindex = 0; // index of array to be used next (equals size of array if array is full)
    Values values;
};


}


#endif

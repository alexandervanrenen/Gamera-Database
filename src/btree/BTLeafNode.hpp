#ifndef BTLEAFNODE_HPP
#define BTLEAFNODE_HPP

#include <array>
#include <cstdint>
#include <iostream>

#include "btree/BTNode.hpp"
#include "btree/BTreeBase.hpp"

namespace dbi {

template <typename Key, typename C>
class BTLeafNode : public BTNode<Key,C> {
public:
    typedef std::pair<Key, TID> Pair;
    typedef std::array<Pair, (PAGESIZE - 2 * sizeof(uint64_t) - 3 * sizeof(PageId)) / (sizeof(Pair))> Values;
    const static uint64_t numkeys = (PAGESIZE - 2 * sizeof(uint64_t) - 3 * sizeof(PageId)) / (sizeof(Pair));
    PageId nextpage = 0; // Pointer to next leaf page (to iterate) (0 signals there is no next page)
    uint64_t nextindex = 0; // index of array to be used next (equals size of array if array is full)
    Values values;
};


}


#endif

#ifndef BTINNERNODE_HPP
#define BTINNERNODE_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <iostream>

#include "btree/BTNode.hpp"
#include "btree/BTreeBase.hpp"

namespace dbi {

template <typename Key, typename C>
class BTInnerNode : public BTNode<Key, C> {
public:
    typedef std::pair<Key, PageId> Pair;
    typedef std::array<Pair, (PAGESIZE - 3 * sizeof(PageId) - 2 * sizeof(uint64_t)) / (sizeof(Pair))> Values;
    const static uint64_t numkeys = (PAGESIZE - 3 * sizeof(PageId) - 2 * sizeof(uint64_t)) / (sizeof(Pair));
    uint64_t nextindex = 0;
    Values values;
    PageId rightpointer;
};


}


#endif

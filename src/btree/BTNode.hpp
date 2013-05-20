#ifndef BTNODE_HPP
#define BTNODE_HPP

#include "btree/BTreeBase.hpp"


namespace dbi {

template <typename Key, typename C>
class BTNode {
public:
    uint64_t nodeType = 0; // 1 = InnerNode, 0 = LeafNode
    static const uint64_t typeInner = 1;
    static const uint64_t typeLeaf = 0;
    PageId pageId = 0;
    PageId parent = 0;
    virtual void dosomething() {}; 
};


}


#endif

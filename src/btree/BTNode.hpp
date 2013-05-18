#ifndef BTNODE_HPP
#define BTNODE_HPP

#include "btree/BTreeBase.hpp"


namespace dbi {

template <typename Key, typename C>
class BTNode {
public:
    PageId pageId = 0;
    PageId parent = 0;
    virtual void dosomething() {};
};


}


#endif

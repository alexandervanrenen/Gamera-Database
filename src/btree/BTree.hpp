#ifndef BTREE_HPP
#define BTREE_HPP

#include <cstdint>
#include <array>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <list>
#include <limits>

#include "btree/BTreeBase.hpp"
#include "btree/BTNode.hpp"
#include "btree/BTLeafNode.hpp"
#include "btree/BTInnerNode.hpp"

namespace dbi {

template <typename Key = uint64_t, typename C = std::less<Key>>
class BTree {
private:
    typedef BTNode<Key, C> Node;
    typedef BTInnerNode<Key, C> InnerNode;
    typedef BTLeafNode<Key, C> LeafNode;
    typedef typename LeafNode::Values::iterator ValuesIterator;
    struct KeyCompare {
            C c{};
        bool operator()(const Key& a, const std::pair<Key, PageId>& b) const {
            return c(a, b.first);
        }
        bool operator()(const std::pair<Key, PageId>& a, const Key& b) const {
            return c(a.first, b);
        }
    };
    struct PairCompare {
            C c{};
        bool operator()(const std::pair<Key, PageId>& a, const std::pair<Key, PageId>& b) const {
            return c(a.first, b.first);
        }
    };
    struct KeyEqual {
        C c{};
        Key k;
        KeyEqual(Key k) {this->k = k;}
        bool operator()(std::pair<Key, PageId>& a) const {
            //return a.first == k;
            return !c(a.first, k) && !c(k, a.first);
        }
    };
    //BTreeSegment* bsm;
    std::unordered_map<PageId, Node*> nodes;
    PageId maxpageId = 0;
    Node* rootnode;
    
    Node* getNode(PageId id) {
        //std::cout << "getNode " << id << std::endl;
        assert(id != 0);
        return nodes.at(id);
    }
    
    void forgetNode(PageId id, bool changed = false) {}
    void forgetNode(Node* node, bool changed = false) {}
    
    PageId newPage(Node* node) {
        nodes.insert({++maxpageId, node});
        return maxpageId;
    }

    LeafNode* newLeafNode() {
        LeafNode* node = new LeafNode();
        node->pageId = newPage(node);
        return node;
    }

    InnerNode* newInnerNode() {
        InnerNode* node = new InnerNode();
        node->pageId = newPage(node);
        return node;
    }

public:
    typedef Key key_type;
    typedef C comparator;

    //BTree(BTreeSegment* bsm) {}
    BTree() {
        LeafNode* ln = new LeafNode();
        nodes.insert({++maxpageId, ln});
        rootnode = ln;
        rootnode->pageId = maxpageId;
    }

    ~BTree() {
    }

    constexpr uint64_t getLeafNodeSize() {
        return LeafNode::numkeys;
    }
    constexpr uint64_t getInnerNodeSize() {
        return InnerNode::numkeys;
    }
    
    template <uint64_t N, typename V = TID>
    void insertLeafValue(std::array<std::pair<Key, V>, N>& values, uint64_t& nextindex, Key k, V tid) {
        //auto it = std::find_if(values.begin(), values.begin()+nextindex, KeyGreater(k));
        auto it = std::upper_bound(values.begin(), values.begin()+nextindex, k, KeyCompare());
        if (it == values.end()) { // insert value at the end
            values[nextindex++] = {k, tid};
        } else { // shift all elements after insert position to make room
            std::move_backward(it, values.begin()+nextindex, values.begin()+nextindex+1);
            *it = {k, tid};
            nextindex++;
        }
    }
    
    bool insert(Key k, TID tid) {
        std::pair<Key, Node*> res = insert(rootnode, k, tid);
        if (res.second == nullptr)
            return true;
        InnerNode* newroot = newInnerNode();
        //std::cout << "Creating new inner node with key " << res.first << ", pageId is " << res.second->pageId << ", old page is " << rootnode->pageId << std::endl;
        PageId rootid = rootnode->pageId;
        newroot->values[0] = {res.first, rootid};
        newroot->rightpointer = res.second->pageId;
        //std::cout << newroot->values[0].first << ", " << newroot->values[0].second << std::endl;
        newroot->nextindex = 1;
        rootnode->parent = newroot->pageId;
        res.second->parent = newroot->pageId;
        //forgetNode(rootid, true);
        //forgetNode(res.seccond, true);
        //std::cout << "First key after root split in node 1: " << dynamic_cast<LeafNode*>(rootnode)->values[0].first << std::endl;
        //std::cout << "First key after root split in node 2: " << dynamic_cast<LeafNode*>(res.second)->values[0].first << std::endl;
        rootnode = newroot;
        return true;
    }

    std::pair<Key, Node*> insert(Node* node, Key k, TID tid) {
        InnerNode* castnode = dynamic_cast<InnerNode*>(node);
        if (castnode != nullptr) { // Node is an inner node (no Leaf node)
            assert(node->pageId != castnode->values[0].second);
            //std::cout << "Searching in innernode " << node->pageId << std::endl;
            //std::cout << castnode->values[0].first << ", " << castnode->values[0].second << std::endl;
            //std::cout << castnode->values[1].first << ", " << castnode->values[1].second << std::endl;
            //std::cout << "rightpointer: " << castnode->rightpointer << std::endl;
            //std::cout << "Searching for key " << k << std::endl;
            auto it = std::upper_bound(castnode->values.begin(), castnode->values.begin()+castnode->nextindex, k, KeyCompare());
            if (it != castnode->values.begin()+castnode->nextindex) {
                //std::cout << "Iterator-Node: " << (*it).first << ", " << (*it).second << std::endl;
                assert((*it).second != 0);
                node = getNode((*it).second);
            }
            else {
                assert(castnode->rightpointer != 0);
                node = getNode(castnode->rightpointer);
            }
            std::pair<Key, Node*> res = insert(node, k, tid);
            if (res.second == nullptr) // no split in sibling node -> nothing to do
                return res;
            if (castnode->nextindex >= castnode->numkeys) { // Overflow
                //std::cout << "!!!Overflow in inner node\n";
                InnerNode* node2 = newInnerNode();
                InnerNode* node1 = castnode;

                uint64_t node1size = InnerNode::numkeys/2;
                uint64_t node2size = InnerNode::numkeys - node1size;
                auto itmiddle = node1->values.begin()+node1size;
                bool insertedAtEnd = false;
                if (it < itmiddle) {
                    //std::cout << "it < itmiddle\n";
                    std::copy(itmiddle, node1->values.end(), node2->values.begin());
                    node2->nextindex = node2size;
                    node1->nextindex = node1size+1;
                    auto itsave = it;
                    PageId tmp = (*it).second;
                    assert(tmp != 0);
                    auto ittarget = std::move_backward(it, node1->values.begin()+node1size, node1->values.begin()+node1->nextindex);
                    (*itsave) = {res.first, tmp};
                    (*ittarget).second = node2->pageId;
                    assert(node2->pageId != 0);
                } else {
                    //std::cout << "it >= itmiddle\n";
                    std::copy(itmiddle, node1->values.end(), node2->values.begin());
                    node2->nextindex = node2size+1;
                    node1->nextindex = node1size;
                    if (it == castnode->values.end()) { // Insert new pointer at end 
                        //std::cout << "At end\n";
                        insertedAtEnd = true;
                        node2->values[node2size] = {res.first, node1->rightpointer};
                        node2->rightpointer = res.second->pageId;
                    } else {
                        it = std::upper_bound(node2->values.begin(), node2->values.begin()+node2size, k, KeyCompare());
                        auto itsave = it;
                        PageId tmp = (*it).second;
                        assert(tmp != 0);
                        auto ittarget = std::move_backward(it, node2->values.begin()+node2size, node2->values.begin()+node2->nextindex);
                        *itsave = {res.first, tmp};
                        (*ittarget).second = node2->pageId;
                        assert(node2->pageId != 0);
                    }
                }
                if (!insertedAtEnd) node2->rightpointer = node1->rightpointer;
                auto itlast = node1->values.begin()+node1->nextindex-1;
                Key tmp = (*itlast).first;
                node1->nextindex--;
                node1->rightpointer = (*itlast).second;
                assert((*itlast).second != 0);
                return {tmp, node2};
            } else {
                if (it == castnode->values.begin()+castnode->nextindex) {
                    //std::cout << "Rightpointer\n";
                    castnode->values[castnode->nextindex++] = {res.first, castnode->rightpointer};
                    castnode->rightpointer = res.second->pageId;
                    return {k, nullptr};
                }
                //std::cout << "Insert in inner node\n";
                InnerNode* node1 = castnode;
                auto itsave = it;
                //std::cout << "Iterator-Node: " << (*it).first << ", " << (*it).second << std::endl;
                PageId tmp = (*it).second;
                assert(tmp != 0);
                auto ittarget = std::move_backward(it, node1->values.begin()+node1->nextindex, node1->values.begin()+node1->nextindex+1);
                *itsave = {res.first, tmp};
                (*ittarget).second = res.second->pageId;
                assert(res.second->pageId != 0);
                node1->nextindex++;
                return {k, nullptr};
            }           

        } 
        // Insert in LeafNode
        LeafNode* leaf = dynamic_cast<LeafNode*>(node);
        assert(leaf != nullptr);
        
        if (leaf->nextindex < leaf->values.size()) { // node is not full
            insertLeafValue<LeafNode::numkeys>(leaf->values, leaf->nextindex, k, tid);
            return {k, nullptr};
        }
        //std::cout << "Overflow in leaf node\n";
        // Node is full -> split
        LeafNode* leaf2 = newLeafNode(); 
        uint64_t leaf1size = (LeafNode::numkeys)/ 2;
        uint64_t leaf2size = (LeafNode::numkeys)-leaf1size;
        
        auto it = std::upper_bound(leaf->values.begin(), leaf->values.begin()+leaf->nextindex, k, KeyCompare());
        auto itmiddle = leaf->values.begin()+leaf1size; // first element to be moved to new leaf
        
        if (it < itmiddle) {
            std::copy(itmiddle, leaf->values.end(), leaf2->values.begin());
            leaf2->nextindex = leaf2size;
            leaf->nextindex = leaf1size;
            insertLeafValue<LeafNode::numkeys>(leaf->values, leaf->nextindex, k, tid);
        } else {
            std::copy(itmiddle, leaf->values.end(), leaf2->values.begin());
            leaf2->nextindex = leaf2size;
            leaf->nextindex = leaf1size;
            insertLeafValue<LeafNode::numkeys>(leaf2->values, leaf2->nextindex, k, tid);
        }
        leaf->nextpage = leaf2->pageId;
        return {(*(leaf2->values.begin())).first, leaf2};
    }


    LeafNode* leafLookup(Key k) {
        Node* node = rootnode;
        InnerNode* castnode = dynamic_cast<InnerNode*>(rootnode);
        while (castnode != nullptr) {
            auto it = std::upper_bound(castnode->values.begin(), castnode->values.begin()+castnode->nextindex, k, KeyCompare());
            if (it != castnode->values.begin()+castnode->nextindex) {
                assert((*it).second != 0);
                node = getNode((*it).second);
            } else {
                assert(castnode->rightpointer != 0);
                node = getNode(castnode->rightpointer);
            }
            castnode = dynamic_cast<InnerNode*>(node);
        }
        LeafNode* leafnode = dynamic_cast<LeafNode*>(node);
        assert(leafnode != nullptr);
        return leafnode;
    }

    bool lookup(Key k, TID& tid) {
        LeafNode* leafnode = leafLookup(k);
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.begin()+leafnode->nextindex, KeyEqual(k));
        if (it != leafnode->values.begin()+leafnode->nextindex) {
            tid = (*it).second;
            return true;
        } else
            return false;
    }

    bool erase(Key k) {
        LeafNode* leafnode = leafLookup(k);
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.begin()+leafnode->nextindex, KeyEqual(k));
        if (it != leafnode->values.begin()+leafnode->nextindex) {
            std::move(it+1, leafnode->values.begin()+leafnode->nextindex, it);
            leafnode->nextindex--;
            forgetNode(leafnode, true);
            // TODO: underflow handling
            return true;
        }
        forgetNode(leafnode, false);
        return false;
    }

    uint64_t size() {
        Node* node = rootnode;
        InnerNode* inode = dynamic_cast<InnerNode*>(node);
        while (inode != nullptr) {
            node = getNode(inode->values[0].second);
            inode = dynamic_cast<InnerNode*>(node);
        }
        LeafNode* leafnode = dynamic_cast<LeafNode*>(node);
        assert(leafnode != nullptr);
        //Key min = std::numeric_limits<Key>::min();
        //LeafNode* leafnode = leafLookup(min);
        uint64_t count = leafnode->nextindex;
        while (leafnode->nextpage != (PageId)0) {
            leafnode = dynamic_cast<LeafNode*>(getNode(leafnode->nextpage));
            count += leafnode->nextindex;
        }
        return count; 
    }


    class Iterator {
        BTree<Key, C>* tree;
        LeafNode* node;
        ValuesIterator it;
        Key last;
        bool finished = false;
        public:
        void next() {
            if (finished) return;
            it++;
            if (it == node->values.begin()+node->nextindex) {
                PageId id = node->nextpage;
                if (id != 0) {
                    node = dynamic_cast<LeafNode*>(tree->getNode(id));
                    it = node->values.begin();
                } else {
                    finished = true;    
                }
            }
            C c;
            if (c(last, (*it).first)) {
                finished = true;
            }
        }

        bool value(Key& key, TID& tid) {
            if (finished || it == node->values.end())
                return false;
            //std::cout << "Key in Iterator: " << (*it).first << std::endl;
            key = (*it).first;
            tid = (*it).second;
            return true;
        }
        Iterator(BTree<Key, C>* pointer, LeafNode* node, ValuesIterator it, Key last) : it(it) {
            tree = pointer;
            this->node = node;
            //this->it = it;
            this->last = last;
        }

    };


    Iterator* lookupRange(Key first, Key last) { 
        LeafNode* leafnode = leafLookup(first);
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.end(), KeyEqual(first));
        return new Iterator{this, leafnode, it, last};
    }

    void visualizeNode(std::ofstream& out, PageId p) {
        assert(p != 0);
        InnerNode* node = dynamic_cast<InnerNode*>(getNode(p));
        if (node != nullptr) {
            visualizeInnerNode(out, node);
        } else {
            LeafNode* leafnode = dynamic_cast<LeafNode*>(getNode(p));
            assert(leafnode != nullptr);
            visualizeLeafNode(out, leafnode);
        }
    }

    void visualizeLeafNode(std::ofstream& out, LeafNode* node) {
        //std::cout << "VisualizeLeafNode " << node->pageId << "\n";
        auto it = node->values.begin();
        out << "node" << node->pageId << " [shape=record, label= \"<begin>("<< node->pageId << ") ";
        while (it != node->values.begin()+node->nextindex) {
            out << " | <key> " << (*it).first; //<< " | <value> " << (*it).second;
            it++;
        }
        out << "\" ];\n";
    }

    void visualizeInnerNode(std::ofstream& out, InnerNode* node) {
        //std::cout << "VisualizeInnerNode " << node->pageId << "\n";
        auto it = node->values.begin();
        std::list<PageId> pages;
        out << "node" << node->pageId << " [shape=record, label= \"<begin>("<< node->pageId << ") | ";
        while (it != node->values.begin()+node->nextindex) {
            out << "<ptr" << (*it).second << "> * | <key> " << (*it).first << " | ";
            pages.push_back((*it).second);
            it++;
        }
        out << "<ptr" << node->rightpointer << ">*\" ];\n\n";
        pages.push_back(node->rightpointer);
        for (PageId p : pages) {
            visualizeNode(out, p);
        }
        for (PageId p : pages) {
            out << "node" << node->pageId << ":ptr" << p << " -> node" << p <<":begin;\n";
        }
    }

    void visualize() {
        std::ofstream out;
        out.open("graph.dot", std::ofstream::out);
        out << "digraph myBTree {\nnode [shape=record];\n";
        visualizeNode(out, rootnode->pageId);
        out << "\n}\n";
        out.close();
    };
};


}


#endif

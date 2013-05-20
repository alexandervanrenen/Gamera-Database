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

#include "segment_manager/BTreeSegment.hpp"
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
    struct KeyEqual {
        C c{};
        Key k;
        KeyEqual(Key k) {this->k = k;}
        bool operator()(std::pair<Key, PageId>& a) const {
            //return a.first == k;
            return !c(a.first, k) && !c(k, a.first);
        }
    };
    
    BTreeSegment& bsm;
    std::unordered_map<PageId, Node*> nodes;
    std::unordered_map<PageId, BufferFrame&> frames;
    std::unordered_map<PageId, bool> locks;
    Node* rootnode = nullptr;
    
    Node* getNode(PageId id, bool exclusive=false) {
        assert(id != 0);
        auto it = nodes.find(id);
        if (it != nodes.end()) {
            return (*it).second;
        } else {
            BufferFrame& frame = bsm.getPage(id, exclusive);
            Node* node = reinterpret_cast<Node*>(frame.getData());
            if (node->nodeType == Node::typeInner) {
                LeafNode* leafnode = static_cast<LeafNode*>(static_cast<void*>(frame.getData()));
                node = leafnode;
            } else {
                InnerNode* innernode = static_cast<InnerNode*>(static_cast<void*>(frame.getData()));
                node = innernode;
            }
            node->pageId = id;
            frames.insert({node->pageId, frame});
            nodes.insert({node->pageId, node});
            //std::cout << "fixOldPage: " << id << std::endl;
            return node;
        }
    }
    
    void releaseNode(PageId id, bool changed = false) {
        auto itnode = nodes.find(id);
        auto itframe = frames.find(id);
        if (itnode == nodes.end() || itframe == frames.end()) {
            assert(false);
        }
        BufferFrame& frame = (*itframe).second;
        nodes.erase(itnode);
        frames.erase(itframe);
        //std::cout << "releasePage: " << id << std::endl;
        bsm.releasePage(frame, changed);
    }
    
    void releaseNode(Node* node, bool changed = false) {
        releaseNode(node->pageId, changed);
    }
    
    Node* newPage(uint64_t nodeType) {
        std::pair<BufferFrame&, PageId> p = bsm.newPage();
        frames.insert({p.second, p.first});
        Node* node;
        if (nodeType == Node::typeLeaf)
            node = reinterpret_cast<LeafNode*>(p.first.getData());
        else if (nodeType == Node::typeInner)
            node = reinterpret_cast<InnerNode*>(p.first.getData());
        node->pageId = p.second;
        nodes.insert({p.second, node});
        //std::cout << "fixNewPage: " << p.second << std::endl;
        return node;
    }

    LeafNode* newLeafNode() {
        Node* node = newPage(Node::typeLeaf);
        node->nodeType = Node::typeLeaf;
        return reinterpret_cast<LeafNode*>(node);
    }

    InnerNode* newInnerNode() {
        Node* node = newPage(Node::typeInner);
        node->nodeType = Node::typeInner;
        return reinterpret_cast<InnerNode*>(node);
    }

    InnerNode* castInner(Node* node) {
        if (node->nodeType == Node::typeInner)
            return static_cast<InnerNode*>(node);
        else
            return nullptr;
    }
    
    LeafNode* castLeaf(Node* node) {
        if (node->nodeType == Node::typeLeaf)
            return static_cast<LeafNode*>(node);
        else
            return nullptr;
    }

    void lockNode(PageId id) {

    }

    void unlockNode(PageId id) {

    }

public:
    typedef Key key_type;
    typedef C comparator;

    BTree(BTreeSegment& bsm) : bsm(bsm) {
        rootnode = getNode(bsm.getRootPage(), true);
    }
    
    ~BTree() {
        releaseNode(rootnode);
        std::cout << "Frames.size(): " << frames.size() << std::endl;
        assert(frames.size() == 0);
    }

    constexpr uint64_t getLeafNodeSize() {
        return LeafNode::numkeys;
    }
    constexpr uint64_t getInnerNodeSize() {
        return InnerNode::numkeys;
    }
    
    template <uint64_t N, typename V = TID>
    void insertLeafValue(std::array<std::pair<Key, V>, N>& values, uint64_t& nextindex, Key k, V tid) {
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
        assert(rootnode != nullptr);
        assert(frames.size() == 1);
        //std::cout << "--------------------------------" << std::endl;
        //std::cout << "RootNode: " << rootnode->pageId << std::endl;
        std::pair<Key, Node*> res = insert(rootnode, k, tid);
        if (res.second == nullptr) {
            assert(frames.size() == 1);
            return true;
        }
        //std::cout << "Overflow in root node, got " << res.second->pageId << std::endl;
        InnerNode* newroot = newInnerNode();
        PageId rootid = rootnode->pageId;
        newroot->values[0] = {res.first, rootid};
        newroot->rightpointer = res.second->pageId;
        newroot->nextindex = 1;
        rootnode->parent = newroot->pageId;
        res.second->parent = newroot->pageId;
        releaseNode(rootnode, true);
        releaseNode(res.second, true);
        rootnode = newroot;
        assert(frames.size() == 1);
        return true;
    }

    std::pair<Key, Node*> insert(Node* innode, Key k, TID tid) {
        InnerNode* castnode = castInner(innode);
        if (castnode != nullptr) { // Node is an inner node (no Leaf node)
            assert(innode->pageId != castnode->values[0].second);
            auto it = std::upper_bound(castnode->values.begin(), castnode->values.begin()+castnode->nextindex, k, KeyCompare());
            Node* node;
            if (it != castnode->values.begin()+castnode->nextindex) { // Not last pointer
                assert((*it).second != 0);
                node = getNode((*it).second);
            }
            else { // last pointer
                assert(castnode->rightpointer != 0);
                node = getNode(castnode->rightpointer);
            }
            std::pair<Key, Node*> res = insert(node, k, tid); // recursive call
            if (res.second == nullptr) { // no split in child node -> nothing to do
                releaseNode(node, true);
                return res;
            }
            if (castnode->nextindex >= castnode->numkeys) { // Overflow -> split node
                //std::cout << "Overflow in inner node" << std::endl;
                InnerNode* node2 = newInnerNode();
                InnerNode* node1 = castnode;

                uint64_t node1size = InnerNode::numkeys/2;
                uint64_t node2size = InnerNode::numkeys - node1size;
                auto itmiddle = node1->values.begin()+node1size;
                bool insertedAtEnd = false;
                if (it < itmiddle) {
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
                releaseNode(node, true);
                releaseNode(res.second, true);
                return {tmp, node2};
            } else { // No overflow -> do a normal insert
                if (it == castnode->values.begin()+castnode->nextindex) { // Child is most right one
                    //std::cout << "InnerNode: Insert at rightpointer" << std::endl;
                    castnode->values[castnode->nextindex++] = {res.first, castnode->rightpointer};
                    castnode->rightpointer = res.second->pageId;
                    releaseNode(res.second, true);
                    releaseNode(node, true);
                    return {k, nullptr};
                }
                //std::cout << "InnerNode: Insert at middle" << std::endl;
                InnerNode* node1 = castnode;
                auto itsave = it;
                PageId tmp = (*it).second;
                assert(tmp != 0);
                auto ittarget = std::move_backward(it, node1->values.begin()+node1->nextindex, node1->values.begin()+node1->nextindex+1);
                *itsave = {res.first, tmp};
                (*ittarget).second = res.second->pageId;
                assert(res.second->pageId != 0);
                node1->nextindex++;
                releaseNode(res.second, true);
                releaseNode(node, true);
                return {k, nullptr};
            }           

        } 
        // Insert in LeafNode
        LeafNode* leaf = castLeaf(innode);
        assert(leaf != nullptr);
        
        if (leaf->nextindex < leaf->values.size()) { // node is not full
            insertLeafValue<LeafNode::numkeys>(leaf->values, leaf->nextindex, k, tid);
            return {k, nullptr};
        }
        
        // Node is full -> split
        //std::cout << "Overflow in leaf node " << leaf->pageId << std::endl;
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
        InnerNode* castnode = castInner(rootnode);
        while (castnode != nullptr) {
            auto it = std::upper_bound(castnode->values.begin(), castnode->values.begin()+castnode->nextindex, k, KeyCompare());
            if (it != castnode->values.begin()+castnode->nextindex) {
                assert((*it).second != 0);
                node = getNode((*it).second);
            } else {
                assert(castnode->rightpointer != 0);
                node = getNode(castnode->rightpointer);
            }
            if (castnode != rootnode) releaseNode(castnode, false);
            castnode = castInner(node);
        }
        LeafNode* leafnode = castLeaf(node);
        assert(leafnode != nullptr);
        return leafnode;
    }

    bool lookup(Key k, TID& tid) {
        LeafNode* leafnode = leafLookup(k);
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.begin()+leafnode->nextindex, KeyEqual(k));
        if (it != leafnode->values.begin()+leafnode->nextindex) {
            tid = (*it).second;
            releaseNode(leafnode, false);
            return true;
        } else {
            releaseNode(leafnode, false);
            return false;
        }
    }

    bool erase(Key k) {
        LeafNode* leafnode = leafLookup(k);
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.begin()+leafnode->nextindex, KeyEqual(k));
        if (it != leafnode->values.begin()+leafnode->nextindex) {
            std::move(it+1, leafnode->values.begin()+leafnode->nextindex, it);
            leafnode->nextindex--;
            releaseNode(leafnode, true);
            // TODO: underflow handling
            return true;
        }
        releaseNode(leafnode, false);
        return false;
    }

    uint64_t size() {
        Node* node = rootnode;
        InnerNode* inode = castInner(node);
        while (inode != nullptr) {
            node = getNode(inode->values[0].second);
            if (inode != rootnode) releaseNode(inode, false);
            inode = castInner(node);
        }
        LeafNode* leafnode = castLeaf(node);
        assert(leafnode != nullptr);
        //Key min = std::numeric_limits<Key>::min();
        //LeafNode* leafnode = leafLookup(min);
        uint64_t count = leafnode->nextindex;
        while (leafnode->nextpage != (PageId)0) {
            PageId next = leafnode->nextpage;
            releaseNode(leafnode, false);
            leafnode = castLeaf(getNode(next));
            count += leafnode->nextindex;
        }
        releaseNode(leafnode, false);
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
                    node = castLeaf(tree->getNode(id));
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
        InnerNode* node = castInner(getNode(p));
        if (node != nullptr) {
            visualizeInnerNode(out, node);
        } else {
            LeafNode* leafnode = castLeaf(getNode(p));
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

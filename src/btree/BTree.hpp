#ifndef BTREE_HPP
#define BTREE_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <stdlib.h>
#include <unordered_map>

#include "btree/BTInnerNode.hpp"
#include "btree/BTLeafNode.hpp"
#include "btree/BTNode.hpp"
#include "btree/BTreeBase.hpp"
#include "segment_manager/BTreeSegment.hpp"

namespace dbi {

template <typename Key = uint64_t, typename C = std::less<Key>>
class BTree {
private:
    typedef BTNode<Key, C> Node;
    typedef BTInnerNode<Key, C> InnerNode;
    typedef BTLeafNode<Key, C> LeafNode;
    typedef typename LeafNode::Values::iterator ValuesIterator;
    typedef typename InnerNode::Values::iterator InnerNodeIterator;

    static_assert(sizeof(InnerNode) <= PAGESIZE, "InnerNode is bigger than a page");
    static_assert(sizeof(LeafNode) <= PAGESIZE, "LeafNode is bigger than a page");

    C c{}; 
    template <typename V = TID>
    struct KeyCompare {
        C c{};
        bool operator()(const Key& a, const std::pair<Key, V>& b) const {
            return c(a, b.first);
        }
        bool operator()(const std::pair<Key, V>& a, const Key& b) const {
            return c(a.first, b);
        }
    };
    template <typename V = PageId>
    struct KeyEqual {
        C c{};
        const Key& k;
        KeyEqual(const Key& key) : k(key) {}
        bool operator()(std::pair<Key, V>& a) const {
            return !c(a.first, k) && !c(k, a.first);
        }
    };
    BTreeSegment& bsm;
    Node* rootnode = nullptr;
    BufferFrame* rootframe = nullptr;
    std::mutex guard;

    std::pair<BufferFrame*, Node*> getNode(PageId id, bool exclusive=kExclusive) {
        assert(id != kMetaPageId);
        BufferFrame& frame = bsm.getPage(id, exclusive);
        Node* node = reinterpret_cast<Node*>(frame.data());
        if (node->nodeType == Node::typeInner) {
            LeafNode* leafnode = static_cast<LeafNode*>(static_cast<void*>(frame.data()));
            node = leafnode;
        } else {
            InnerNode* innernode = static_cast<InnerNode*>(static_cast<void*>(frame.data()));
            node = innernode;
        }
        node->pageId = id;
        return {&frame, node};
    }
    
    std::pair<BufferFrame*, Node*> newPage(uint64_t nodeType) {
        std::pair<BufferFrame&, PageId> p = bsm.newPage();
        Node* node;
        if (nodeType == Node::typeLeaf)
            node = reinterpret_cast<LeafNode*>(p.first.data());
        else if (nodeType == Node::typeInner)
            node = reinterpret_cast<InnerNode*>(p.first.data());
        else
            assert(false && "unknown nodeType");
        node->pageId = p.second;
        node->nodeType = nodeType;
        return {&p.first, node};
    }

    std::pair<BufferFrame*, LeafNode*> newLeafNode() {
        std::pair<BufferFrame*, Node*> p = newPage(Node::typeLeaf);
        return {p.first, reinterpret_cast<LeafNode*>(p.second)};
        //return newPage(Node::typeLeaf);
        //return reinterpret_cast<LeafNode*>(node);
    }

    std::pair<BufferFrame*, InnerNode*> newInnerNode() {
        std::pair<BufferFrame*, Node*> p = newPage(Node::typeInner);
        return {p.first, reinterpret_cast<InnerNode*>(p.second)};
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

    void releaseNode(BufferFrame* frame, bool isDirty=true) {
        if (frame == rootframe) {
            guard.unlock();
        } else {
            bsm.releasePage(*frame, isDirty);
        }
    }

    //void releaseNode(BufferFrame* frame, bool isDirty=true) {
    //    releaseNode(*frame, isDirty);
    //}

public:
    typedef Key key_type;
    typedef C comparator;

    BTree(BTreeSegment& bsm) : bsm(bsm), guard() {
        std::pair<BufferFrame*, Node*> p = getNode(bsm.getRootPage());    
        rootframe = p.first;
        rootnode = p.second;
    }
    
    ~BTree() {
        bsm.releasePage(*rootframe, true);
    }

    constexpr uint64_t getLeafNodeSize() {
        return LeafNode::numkeys;
    }
    constexpr uint64_t getInnerNodeSize() {
        return InnerNode::numkeys;
    }
    
    template <uint64_t N, typename V = TID>
    void insertLeafValue(std::array<std::pair<Key, V>, N>& values, uint64_t& nextindex, const Key& k, const V& tid) {
        auto it = std::upper_bound(values.begin(), values.begin()+nextindex, k, KeyCompare<V>());
        if (it == values.end()) { // insert value at the end
            values[nextindex++] = {k, tid};
        } else { // shift all elements after insert position to make room
            std::move_backward(it, values.begin()+nextindex, values.begin()+nextindex+1);
            *it = {k, tid};
            nextindex++;
        }
    }
    
    bool insert(const Key& k, const TID& tid) {
        assert(rootnode != nullptr);
        guard.lock();
        Node* node = rootnode;
        BufferFrame* nodeframe = rootframe;
        Node* parentnode = nullptr;
        BufferFrame* parentframe = nullptr;
        InnerNodeIterator it;
        while (true) {
            InnerNode* innernode = castInner(node);
            if (innernode != nullptr) { // Node is an inner node (no Leaf node)
                if (innernode->nextindex >= innernode->numkeys) { // Possible overflow -> preemptive split
                    //std::cout << "Overflow in inner node\n";
                    std::pair<BufferFrame*, InnerNode*> p = newInnerNode();
                    Key up = splitInnerNode(innernode, p.second);
                    if (parentnode == nullptr) { // Split node was root node -> new root
                        std::pair<BufferFrame*, InnerNode*> proot = newInnerNode();
                        proot.second->values[0] = {up, rootnode->pageId};
                        proot.second->rightpointer = p.second->pageId;
                        proot.second->nextindex = 1;
                        rootnode = proot.second;
                        rootframe = proot.first;
                        guard.unlock();
                        parentframe = nullptr;
                    } else {
                        InnerNode* innerparent = castInner(parentnode);
                        if (it == innerparent->values.begin()+innerparent->nextindex) { // insert value at the end
                            innerparent->values[innerparent->nextindex++] = {up, innerparent->rightpointer};
                            innerparent->rightpointer = p.second->pageId;
                        } else { // shift all elements after insert position to make room
                            auto itsave = it;
                            PageId tmp = (*it).second;
                            auto ittarget = std::move_backward(it, innerparent->values.begin()+innerparent->nextindex, innerparent->values.begin()+innerparent->nextindex+1);
                            (*itsave) = {up, tmp};
                            (*ittarget).second = p.second->pageId;
                            innerparent->nextindex++;
                        }
                        //insertLeafValue<InnerNode::numkeys, PageId>(innerparent->values, innernode->nextindex, up, p.second->pageId);
                    }
                    if (c(k, up)) { // continue in left node 
                        bsm.releasePage(*p.first, true); // release right node
                    } else { // continue in right node
                        bsm.releasePage(*nodeframe, true); // release left node 
                        nodeframe = p.first;
                        innernode = p.second;
                        node = p.second;
                    }
                }
                if (parentframe != nullptr) releaseNode(parentframe, true);
                PageId childId;
                // Find correct child
                it = std::upper_bound(innernode->values.begin(), innernode->values.begin()+innernode->nextindex, k, KeyCompare<PageId>());
                if (it != innernode->values.begin()+innernode->nextindex) { // Not last pointer
                    assert((*it).second != kMetaPageId);
                    childId = (*it).second;
                } else { // last pointer
                    assert(innernode->rightpointer != kMetaPageId);
                    childId = innernode->rightpointer;
                }
                std::pair<BufferFrame*, Node*> p = getNode(childId, true);
                parentframe = nodeframe;
                parentnode = node;
                node = p.second;
                nodeframe = p.first;
            } else { // Node is a LeafNode
                LeafNode* leaf = castLeaf(node);
                assert(leaf != nullptr);
                std::pair<Key, PageId> res = insertInLeaf(leaf, k, tid);
                if (res.second != kMetaPageId) { // overflow in leaf
                    //std::cout << "Overflow in leaf node\n";
                    if (parentnode == nullptr) { // Leaf is root node
                        std::pair<BufferFrame*, InnerNode*> proot = newInnerNode();
                        proot.second->values[0] = {res.first, rootnode->pageId};
                        proot.second->rightpointer = res.second;
                        proot.second->nextindex = 1;
                        rootnode = proot.second;
                        rootframe = proot.first;
                        guard.unlock();
                        releaseNode(nodeframe, true);
                    } else { // Leaf was not root node -> insert split key in parent node
                        //std::cout << "Leaf was not root\n";
                        InnerNode* innerparent = castInner(parentnode);
                        if (it == innerparent->values.begin()+innerparent->nextindex) { // insert value at the end
                            innerparent->values[innerparent->nextindex++] = {res.first, innerparent->rightpointer};
                            innerparent->rightpointer = res.second;
                        } else { // shift all elements after insert position to make room
                            auto itsave = it;
                            PageId tmp = (*it).second;
                            auto ittarget = std::move_backward(it, innerparent->values.begin()+innerparent->nextindex, innerparent->values.begin()+innerparent->nextindex+1);
                            (*itsave) = {res.first, tmp};
                            (*ittarget).second = res.second;
                            innerparent->nextindex++;
                        }
                        //insertLeafValue<InnerNode::numkeys, PageId>(innerparent->values, innerparent->nextindex, res.first, res.second);
                        releaseNode(parentframe, true);
                        releaseNode(nodeframe, true);
                    }
                } else {
                    // release Nodes
                    if (parentframe != nullptr) releaseNode(parentframe, true);
                    releaseNode(nodeframe, true);
                }
                return true;
            }
        }
    }


    Key splitInnerNode(InnerNode* node1, InnerNode* node2) {
        uint64_t node1size = InnerNode::numkeys/2 + 1;
        uint64_t node2size = InnerNode::numkeys - node1size;
        auto itmiddle = node1->values.begin()+node1size; // first value to be copied to second node
        auto itlastnode1 = itmiddle; // last element in first node after split
        itlastnode1--;
        std::copy(itmiddle, node1->values.end(), node2->values.begin());
        node2->nextindex = node2size;
        node1->nextindex = node1size-1;
        node2->rightpointer = node1->rightpointer;
        node1->rightpointer = (*itlastnode1).second;
        Key up = (*itlastnode1).first;
        return up;
    }


    std::pair<Key, PageId> insertInLeaf(LeafNode* leaf, const Key& k, const TID& tid) { 
        assert(leaf != nullptr);
        if (leaf->nextindex < leaf->values.size()) { // node is not full
            insertLeafValue<LeafNode::numkeys>(leaf->values, leaf->nextindex, k, tid);
            return {k, kMetaPageId};
        }
        // Node is full -> split
        //std::cout << "Overflow in leaf node " << leaf->pageId << std::endl;
        auto p = newLeafNode();
        LeafNode* leaf2 = p.second;
        uint64_t leaf1size = (LeafNode::numkeys)/ 2;
        uint64_t leaf2size = (LeafNode::numkeys)-leaf1size;
        auto it = std::upper_bound(leaf->values.begin(), leaf->values.begin()+leaf->nextindex, k, KeyCompare<TID>()); // find insert position
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
        PageId id = leaf2->pageId;
        Key up = (*(leaf2->values.begin())).first;
        bsm.releasePage(*p.first, true); // Release leaf2
        return {up, id};
    }


    std::pair<BufferFrame*, LeafNode*> leafLookup(const Key& k) {
        assert(rootnode != nullptr);
        guard.lock();
        Node* node = rootnode;
        BufferFrame* nodeframe = rootframe;
        BufferFrame* parentframe = nullptr;
        
        InnerNode* castnode = castInner(rootnode);
        while (castnode != nullptr) {
            auto it = std::upper_bound(castnode->values.begin(), castnode->values.begin()+castnode->nextindex, k, KeyCompare<PageId>());
            PageId childId;
            if (it != castnode->values.begin()+castnode->nextindex) {
                assert((*it).second != kMetaPageId);
                childId = (*it).second;
            } else {
                assert(castnode->rightpointer != kMetaPageId);
                childId = castnode->rightpointer;
            }
            std::pair<BufferFrame*, Node*> p = getNode(childId);
            if (parentframe != nullptr) releaseNode(parentframe, false);
            parentframe = nodeframe;
            nodeframe = p.first;
            node = p.second;
            castnode = castInner(node);
        }
        LeafNode* leafnode = castLeaf(node);
        assert(leafnode != nullptr);
        if (parentframe != nullptr) releaseNode(parentframe, false);
        return {nodeframe, leafnode};
    }

    bool lookup(const Key& k, TID& tid) {
        std::pair<BufferFrame*, LeafNode*> p = leafLookup(k);
        LeafNode* leafnode = p.second;
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.begin()+leafnode->nextindex, KeyEqual<TID>(k));
        if (it != leafnode->values.begin()+leafnode->nextindex) {
            tid = (*it).second;
            releaseNode(p.first, false);
            return true;
        } else {
            releaseNode(p.first, false);
            return false;
        }
    }


    bool erase(const Key& k) {
        std::pair<BufferFrame*, LeafNode*> p = leafLookup(k);
        LeafNode* leafnode = p.second;
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.begin()+leafnode->nextindex, KeyEqual<TID>(k));
        if (it != leafnode->values.begin()+leafnode->nextindex) {
            std::move(it+1, leafnode->values.begin()+leafnode->nextindex, it);
            leafnode->nextindex--;
            releaseNode(p.first, true);
            // TODO: underflow handling
            return true;
        }
        releaseNode(p.first, false);
        return false;
    }


    uint64_t size() {
        assert(rootnode != nullptr);
        guard.lock();
        Node* node = rootnode;
        BufferFrame* nodeframe = rootframe;
        BufferFrame* parentframe = nullptr;
        
        InnerNode* inode = castInner(node);
        while (inode != nullptr) {
            PageId childId = inode->values[0].second;
            std::pair<BufferFrame*, Node*> p = getNode(childId);
            if (parentframe != nullptr) releaseNode(parentframe, false);
            parentframe = nodeframe;
            nodeframe = p.first;
            node = p.second;
            inode = castInner(node);
        }
        if (parentframe != nullptr) releaseNode(parentframe, false);
        LeafNode* leafnode = castLeaf(node);
        assert(leafnode != nullptr);
     
        uint64_t count = leafnode->nextindex;
        while (leafnode->nextpage != kMetaPageId) {
            PageId next = leafnode->nextpage;
            std::pair<BufferFrame*, Node*> p = getNode(next);
            releaseNode(nodeframe, false);
            nodeframe = p.first;
            node = p.second;
            leafnode = castLeaf(node);
            count += leafnode->nextindex;
        }
        releaseNode(nodeframe, false);
        return count; 
    }


    class Iterator {
        BTree<Key, C>* tree;
        LeafNode* node;
        BufferFrame* frame;
        ValuesIterator it;
        const Key& last;
        bool finished = false;
        public:

        bool operator==(const Iterator& other) const {
            return (finished && other.finished) || (finished == other.finished) && frame == other.frame && it == other.it ;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        void next() {
            if (finished) return;
            it++;
            if (it == node->values.begin()+node->nextindex) {
                PageId id = node->nextpage;
                tree->releaseNode(frame, false);
                if (id != kMetaPageId) {
                    std::pair<BufferFrame*, Node*> p = tree->getNode(id);
                    node = tree->castLeaf(p.second);
                    frame = p.first;
                    it = node->values.begin();
                } else {
                    finished = true;
                    frame = nullptr;
                    node = nullptr; 
                }
            }
            C c;
            if (c(last, (*it).first)) { // next value smaller than last in Range, finish
                finished = true;
            }
        }
        const Iterator operator++() {
            Iterator result = *this;
            next();
            return result;
        }

        const Iterator& operator++(int) {
            next();
            return *this;
        }


        std::pair<Key, TID> operator*() const {
            //if (finished || it == node->values.end())
                //return;
            return {(*it).first, (*it).second};
        }

        bool valid () const {
            return !finished;
        }

        Iterator(BTree<Key, C>* pointer, BufferFrame* frame, LeafNode* node, ValuesIterator it, const Key& last) : it(it), last(last) {
            tree = pointer;
            this->node = node;
            this->frame = frame;
        }

        Iterator(BTree<Key, C>* pointer, const Key& last) : last(last) {
            tree = pointer;
            finished = true;
            this->node = nullptr;
            this->frame = nullptr;
        }

        ~Iterator() {
            if (frame != nullptr)
                tree->releaseNode(frame, false);
        }

    };


    Iterator lookupRange(const Key& first, const Key& last) { 
        std::pair<BufferFrame*, LeafNode*> p = leafLookup(first);
        LeafNode* leafnode = p.second;
        auto it = std::find_if(leafnode->values.begin(), leafnode->values.begin()+leafnode->nextindex, KeyEqual<TID>(first));
        if (it != leafnode->values.begin()+leafnode->nextindex)
            return *new Iterator{this, p.first, leafnode, it, last};
        else
            return *new Iterator{this, last};
    }

    Iterator* end() {
        return new Iterator{this, Key()};
    }
    
    void visualizeNode(std::ofstream& out, PageId id) {
        assert(id != kMetaPageId);
        std::pair<BufferFrame*, Node*> p;
        if (id == rootnode->pageId)
            p = {rootframe, rootnode};
        else
            p = getNode(id);
        InnerNode* node = castInner(p.second);
        if (node != nullptr) {
            visualizeInnerNode(out, node, p.first);
        } else {
            LeafNode* leafnode = castLeaf(p.second);
            assert(leafnode != nullptr);
            visualizeLeafNode(out, leafnode, p.first);
        }
    }

    void visualizeLeafNode(std::ofstream& out, LeafNode* node, BufferFrame* frame) {
        auto it = node->values.begin();
        out << "node" << node->pageId << " [shape=record, label= \"<begin>("<< node->pageId << ") ";
        while (it != node->values.begin()+node->nextindex) {
            out << " | <key> " << (*it).first; //<< " | <value> " << (*it).second;
            it++;
        }
        out << "\" ];\n";
        releaseNode(frame, false);
    }

    void visualizeInnerNode(std::ofstream& out, InnerNode* node, BufferFrame* frame) {
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
        releaseNode(frame, false);
        for (PageId p : pages) {
            visualizeNode(out, p);
        }
        for (PageId p : pages) {
            out << "node" << node->pageId << ":ptr" << p << " -> node" << p <<":begin;\n";
        }
    }

    void visualize(std::string filename = "bin/var/tree.dot") {
        std::ofstream out;
        out.open(filename.c_str(), std::ofstream::out);
        out << "digraph myBTree {\nnode [shape=record];\n";
        guard.lock();
        visualizeNode(out, rootnode->pageId);
        out << "\n}\n";
        out.close();
        if (system("dot -Tpng bin/var/tree.dot -o bin/var/tree.png") != 0) {
            std::cout << "Converting tree.dot to a png-File failed" << std::endl;
        }
    }
};


}


#endif

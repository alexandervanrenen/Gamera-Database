#pragma once

#include <iostream>
//#include <vector>
//#include <unordered_map>
#include "tbb/concurrent_unordered_map.h"

namespace dbi {

template <typename Key>//, Key invalid>
class Graph {
private:
    typedef tbb::concurrent_unordered_map<Key, bool> KeyMap;
    typedef tbb::concurrent_unordered_map<Key, KeyMap> EdgeMap;
    //typedef tbb::concurrent_vector<Key> KeyVector;
    EdgeMap edges;

public:
    void addEdge(Key from, Key to) {
        auto it = edges.find(from);
        if (it == edges.end()) {
            auto p = edges.insert({from, KeyMap()});
            it = p.first;
        }
        KeyMap& k = (*it).second;
        auto pk = k.insert({to, true});
        if (!pk.second)
            (*pk.first).second = true;
    };

    void removeEdge(Key from, Key to) {
        auto it = edges.find(from);
        assert(it != edges.end());
        KeyMap& k = (*it).second;
        auto itk = k.find(to);
        assert(itk != k.end());
        (*itk).second = false;
        //k.unsafe_erase(itk);
    };

    void clearNode(Key key) {
        auto it = edges.find(key);
        if(it == edges.end())
            return;
        KeyMap& k = (*it).second;
        //k.clear();
        for (auto p : k) {
            if (p.second) p.second = false;
        }
    }

    bool hasCycle(Key key) {
        KeyMap m = KeyMap();
        return hasCycle(key, m);
    };

    bool hasCycle(Key key, KeyMap visited) {
        auto it = edges.find(key);
        if(it == edges.end())
            return false;
        KeyMap& k = (*it).second;
        //for (auto p : k) {
        for (auto it = k.begin(); it != k.end(); it++) {
            auto to = (*it).first;
            if (!(*it).second) continue;
            if (visited.find(to) != visited.end())
                return true;
            visited.insert({to, true});
            if (hasCycle(to, visited))
                return true;
            //if (k.empty()) return false;
            assert(!k.empty());
        }
        return false;
    };

};

}

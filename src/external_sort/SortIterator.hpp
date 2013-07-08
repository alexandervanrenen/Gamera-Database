#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstring>

namespace dbi {

class SortIterator : public std::iterator<std::bidirectional_iterator_tag, SortIterator> {
public:
    typedef char* value_type;

    SortIterator() {};

    SortIterator(const char* data, uint64_t keysize) : data((char*)data), keysize(keysize){};

    SortIterator& operator=(const SortIterator& other) {
        data = other.data;
        keysize = other.keysize;
        return *this;
    }
 

    char* operator*() const {
        return data;
    };
    
    const SortIterator operator++() { // i++
        SortIterator it = *this;
        data += keysize;
        return it;
    }

    const SortIterator& operator++(int) { //++i
        data += keysize;
        return *this;
    }
    
    const SortIterator& operator--() { //--i
        data -= keysize;
        return *this;
    }

    const SortIterator operator+(uint64_t count) {
        SortIterator it = *this;
        it.data += count * keysize;
        return it;
    }
    
    //int operator+(const SortIterator& other) {
    //    return data+(char*)other.data;
    //}
    
    const SortIterator operator-(uint64_t count) {
        SortIterator it = *this;
        it.data -= count * keysize;
        return it;
    }
    
    int operator-(const SortIterator& other) {
        return data - other.data;
    }

    //const SortIterator operator>>(int i) {
    //}

    //char* operator[](int i) {
    //    return *(this + i);
    //}
    
    const SortIterator& operator+=(uint64_t count) {
        data += count * keysize;
        return *this;
    }

    bool operator==(const SortIterator& other) const {
        return data == other.data;
    }

    bool operator!=(const SortIterator& other) const {
        return !(*this==other);
    }
    
    bool operator<(const SortIterator& other) const {
        return data < other.data;
    }

    uint64_t bytes() const {
        return keysize;
    }


private:
    char* data;
    uint64_t keysize;
};

template <typename I, typename Compare>
void sort(I first, I last, Compare& c, uint64_t bytes) {
    char* tmp = new char[bytes];
    while (last != first) {
        I cur = first;
        while (cur != last) {
            I next = cur+1;
            if (c.less(*(next), *cur)) {
                //std::cout << "Swapping values\n";
                std::memcpy(tmp, *cur, bytes);
                std::memcpy(*cur, *(cur+1), bytes);
                std::memcpy(*(cur+1), tmp, bytes);
                //std::cout << "Swapped values\n";
                //std::swap_ranges(*cur, *(cur+1), *(cur+1));
            }
            ++cur;
        }
        --last;
    }
    delete[] tmp;
}



}

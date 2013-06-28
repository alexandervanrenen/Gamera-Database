#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>

namespace dbi {

template <typename Value>
class CharIterator : public std::iterator<std::bidirectional_iterator_tag, CharIterator<Value>> {
public:
    typedef char* value_type;

    CharIterator() {};

    CharIterator(const char* data, uint64_t keysize, uint64_t maxindex) : data((char*)data), keysize(keysize), nextindex(0), maxindex(maxindex), pairsize(keysize+valuesize) {};
    
    CharIterator(const char* data, uint64_t keysize, uint64_t maxindex, uint64_t nextindex) : data((char*)data), keysize(keysize), nextindex(nextindex), maxindex(maxindex), pairsize(keysize+valuesize) {};

    CharIterator& operator=(const CharIterator& other) {
        data = other.data;
        keysize = other.keysize;
        nextindex = other.nextindex;
        maxindex = other.maxindex;
        pairsize = other.pairsize;
        return *this;
    }
 

    char* operator*() const {
        return data+pairsize*nextindex;
    };
    
    const CharIterator operator++() { // i++
        CharIterator it = *this;
        nextindex++;
        return it;
    }

    const CharIterator& operator++(int) { //++i
        nextindex++;
        return *this;
    }
    
    const CharIterator& operator--() { //--i
        --nextindex;
        return *this;
    }

    const CharIterator operator+(uint64_t count) {
        CharIterator it = *this;
        it.nextindex += count;
        return it;
    }
    
    int operator+(const CharIterator& other) {
        return nextindex + other.nextindex;
    }
    
    const CharIterator operator-(uint64_t count) {
        CharIterator it = *this;
        it.nextindex -= count;
        return it;
    }
    
    int operator-(const CharIterator& other) {
        return nextindex - other.nextindex;
    }

    //const CharIterator operator>>(int i) {
    //}

    char* operator[](int i) {
        return *(this + i);
    }
    
    const CharIterator& operator+=(uint64_t count) {
        nextindex += count;
        return *this;
    }

    bool operator==(const CharIterator& other) const {
        return data == other.data && (nextindex == other.nextindex || (nextindex >= maxindex && other.nextindex >= other.maxindex));
    }

    bool operator!=(const CharIterator& other) const {
        return !(*this==other);
    }
    
    bool operator<(const CharIterator& other) const {
        return nextindex < other.nextindex;
    }

    char* pointer() const {
        return data+pairsize*nextindex;
    }

    char* pointernext() const {
        return data+pairsize*(nextindex+1);
    }
    
    uint64_t index() const {
        return nextindex;
    }

    uint64_t getPairSize() const {
        return pairsize;
    }
    
    void second(Value& v) {
        *(Value*)(pointer()+keysize) = v;
    }

    Value second() const {
        return *(Value*)(pointer()+keysize);
    }

private:
    char* data;
    uint64_t datasize;
    uint64_t keysize;
    uint64_t nextindex;
    uint64_t maxindex; // lastindex+1
    uint64_t pairsize;
    static const uint64_t valuesize = sizeof(Value);
};



}

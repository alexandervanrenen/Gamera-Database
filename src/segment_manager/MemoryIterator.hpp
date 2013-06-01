#pragma once

#include <cstring>
#include "PageIDIterator.hpp"
#include "Segment.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "buffer_manager/BufferFrame.hpp"
#include "common/Config.hpp"

namespace dbi {

class MemoryIterator {
public:
    MemoryIterator(BufferManager& bm, PageIDIterator it) : bm(bm), it(it) {
        curframe = &bm.fixPage(*it, kExclusive);
        ptr = curframe->getData();
        offset = 0;
    }
    
    void close() {
        bm.unfixPage(*curframe, modified);
    }

    char* getNextBytes(const uint64_t numbytes) {
        if (offset + numbytes < kPageSize) {
            char* p = ptr;
            ptr += numbytes;
            offset += numbytes;
            return p;
        } else {
            char* area = new char[numbytes];
            uint64_t diff = kPageSize - offset;
            memcpy(area, ptr, diff);
            bm.unfixPage(*curframe, modified);
            curframe = &bm.fixPage(*(++it), kExclusive);
            modified = false;
            ptr = curframe->getData();
            memcpy(area+diff, ptr, numbytes-diff);
            offset = numbytes-diff;
            ptr += numbytes-diff;
            return area;
        }
    }

    uint64_t getUint64() {
        return *((uint64_t*)getNextBytes(sizeof(uint64_t)));
    }
    
    uint32_t getUint32() {
        return *((uint32_t*)getNextBytes(sizeof(uint32_t)));
    }
    
    uint8_t getUint8() {
        return *((uint8_t*)getNextBytes(sizeof(uint8_t)));
    }


    void saveBytes(char* data, const uint64_t len) {
        modified = true;
        if (offset + len < kPageSize) {
            memcpy(ptr, data, len);
            ptr += len;
            offset += len;
        } else {
            uint64_t diff = kPageSize - offset;
            memcpy(ptr, data, diff);
            bm.unfixPage(*curframe, modified);
            curframe = &bm.fixPage(*(++it), kExclusive);
            ptr = curframe->getData();
            memcpy(ptr, data+diff, len-diff);
            offset = len-diff;
            ptr += len-diff;
        }
    }

    void saveUint64(uint64_t value) {
        saveBytes((char*)&value, sizeof(uint64_t));
    }
    
    void saveUint32(uint32_t value) {
        saveBytes((char*)&value, sizeof(uint32_t));
    }

    void saveUint8(uint8_t value) {
        saveBytes((char*)&value, sizeof(uint8_t));
    }


private:
    BufferManager& bm;
    PageIDIterator it;
    BufferFrame* curframe;
    char* ptr;
    uint64_t offset;
    bool modified = false;
};

}

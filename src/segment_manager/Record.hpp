#pragma once

#include <string.h>

namespace dbi {

class Record {
   const char* const mem;
   uint32_t length;

public:
   Record& operator=(Record& rhs) = delete;
   Record(Record& t) = delete;
   Record(Record&& t) = delete;

   explicit Record(const char* const dataIn, unsigned length) : mem(dataIn), length(length) {}

   const char* data() const {return mem;}

   uint32_t size() const {return length;}
};

}

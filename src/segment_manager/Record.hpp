#pragma once

#include <string.h>

namespace dbi {

class Record {
   char* data;
   uint32_t length;

public:
   Record& operator=(Record& rhs) = delete;
   Record(Record& t) = delete;
   Record(Record&& t) = delete;

   explicit Record(const char* const ptr, unsigned len) : data(data), length(length) {}

   const char* data() const {
      return data;
   }

   uint32_t size() const {
      return length;
   }
};

}

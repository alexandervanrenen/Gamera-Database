#pragma once

#include "PageId.hpp"
#include "RecordId.hpp"
#include <cstdint>
#include <sstream>

namespace dbi {

class TupleId {
public:
   constexpr explicit TupleId() : tid(-1) {}
   constexpr explicit TupleId(uint64_t tid) : tid(tid) {}
   constexpr explicit TupleId(const PageId pid, const RecordId rid) : tid((pid.pid << 16) + rid.rid) {}
   constexpr TupleId(const TupleId& other) : tid(other.tid) {}
   const TupleId& operator= (const TupleId other) {tid = other.tid; return *this;}

   PageId toPageId() const {return PageId(tid >> 16);}
   RecordId toRecordId() const {return RecordId(tid & 0xffff);}

   constexpr bool operator== (const TupleId& other) const {return tid == other.tid;}
   constexpr bool operator!= (const TupleId& other) const {return tid != other.tid;}

   constexpr uint64_t toInteger() const {return tid;}

   friend std::ostream& operator<< (std::ostream& out, TupleId& tid) {return out << tid.tid;}

private:
   uint64_t tid;
};

}

namespace std {

template<>
struct hash<dbi::TupleId> {
   constexpr size_t operator()(const dbi::TupleId& pid) const {
      return pid.toInteger();
   }
};

}

#pragma once

#include <cstdint>
#include <sstream>
#include <functional>

namespace dbi {

class SegmentId {
public:
   constexpr explicit SegmentId() : sid(-1) {}
   constexpr explicit SegmentId(uint64_t sid) : sid(sid) {}
   constexpr SegmentId(const SegmentId& other) : sid(other.sid) {}
   const SegmentId& operator= (const SegmentId& other) {sid = other.sid; return *this;}

   constexpr bool operator== (const SegmentId& other) const {return sid == other.sid;}
   constexpr bool operator!= (const SegmentId& other) const {return sid != other.sid;}

   const SegmentId operator++() {return SegmentId(sid++);}
   const SegmentId& operator++(int) {sid++; return *this;}

   friend std::ostream& operator<< (std::ostream& out, const SegmentId& sid) {return out << sid;}

   constexpr uint64_t toInteger() const {return sid;}

private:
   uint64_t sid;
};

}

namespace std {

template<>
struct hash<dbi::SegmentId> {
   constexpr size_t operator()(const dbi::SegmentId& sid) const {
      return sid.toInteger();
   }
};

}

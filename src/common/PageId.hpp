#pragma once

#include <cstdint>
#include <sstream>
#include <functional>

namespace dbi {

class PageId {
public:
   constexpr explicit PageId() : pid(-1) {}
   constexpr explicit PageId(uint64_t pid) : pid(pid) {}
   constexpr PageId(const PageId& other) : pid(other.pid) {}
   const PageId& operator= (const PageId& other) {pid = other.pid; return *this;}

   constexpr bool operator== (const PageId& other) const {return pid == other.pid;}
   constexpr bool operator!= (const PageId& other) const {return pid != other.pid;}

   const PageId operator++() {return PageId(pid++);}
   const PageId& operator++(int) {pid++; return *this;}

   friend std::ostream& operator<< (std::ostream& out, const PageId& pid) {return out << pid;}

   constexpr uint64_t toInteger() const {return pid;}

private:
   uint64_t pid;
   friend class TupleId;
};

}

namespace std {

template<>
struct hash<dbi::PageId> {
   constexpr size_t operator()(const dbi::PageId& pid) const {
      return pid.toInteger();
   }
};

}

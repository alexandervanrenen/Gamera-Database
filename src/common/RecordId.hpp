#pragma once

#include <cstdint>
#include <sstream>

namespace dbi {

class RecordId {
public:
   constexpr explicit RecordId(uint16_t rid) : rid(rid) {}
   constexpr RecordId(const RecordId& other) : rid(other.rid) {}
   const RecordId& operator= (const RecordId& other) {rid = other.rid; return *this;}

   constexpr bool operator== (const RecordId& other) const {return rid == other.rid;}
   constexpr bool operator!= (const RecordId& other) const {return rid != other.rid;}

   friend std::ostream& operator<< (std::ostream& out, RecordId& rid) {return out << rid.rid;}

   constexpr uint16_t toInteger() const {return rid;}

private:
   uint16_t rid;
   friend class TupleId;
};

}

namespace std {

template<>
struct hash<dbi::RecordId> {
   constexpr size_t operator()(const dbi::RecordId& pid) const {
      return pid.toInteger();
   }
};

}

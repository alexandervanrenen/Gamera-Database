#pragma once

#include <ios>
#include <memory>
#include <set>
#include <vector>

namespace harriet { class Expression; }

namespace dbi {

namespace qopt {

class ColumnAccessInfo;

struct Predicate {
   ~Predicate();

   /// The columns the predicate depends on
   std::set<ColumnAccessInfo> requiredColumns;
   /// The actual condition
   std::unique_ptr<harriet::Expression> condition;

   /// Find out all tables needed for this predicate
   std::set<uint32_t> getRequiredTables() const;

   /// Write to the given stream
   void dump(std::ostream& os) const;
};

}

}

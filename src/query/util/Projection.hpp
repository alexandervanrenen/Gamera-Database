#pragma once

#include <set>
#include <memory>
#include <string>
#include <cstdint>

namespace harriet { class Expression; }

namespace dbi {

namespace qopt {

class ColumnAccessInfo;

class Projection {
public:
   Projection(std::unique_ptr<harriet::Expression> expression);
   ~Projection();

   /// The columns the projection depends on
   std::set<ColumnAccessInfo> requiredColumns;
   /// The actual expression
   std::unique_ptr<harriet::Expression> expression;
   std::string prettyExpression;

   /// The name of the projection
   std::string alias;
   uint32_t resultRegisterSlot;

   /// Find out all tables needed for this predicate
   std::set<uint32_t> getRequiredTables() const;

   /// Write to the given stream
   void dump(std::ostream& os) const;
};

}

}

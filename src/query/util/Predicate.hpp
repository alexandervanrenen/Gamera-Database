#pragma once

#include "query/util/ColumnAccessInfo.hpp"
#include <ios>
#include <memory>
#include <set>
#include <vector>

namespace harriet { class Expression; }

namespace dbi {

namespace qopt {

struct Predicate {
   std::vector<ColumnAccessInfo> columns;
   std::set<uint32_t> tables;
   std::unique_ptr<harriet::Expression> condition;

   void dump(std::ostream& os) const;
};

}

}

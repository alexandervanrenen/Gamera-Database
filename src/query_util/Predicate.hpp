#pragma once

#include "query_util/ColumnAccessInfo.hpp"
#include <memory>
#include <vector>
#include <ios>
#include <set>

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

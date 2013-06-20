#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace harriet { class Expression; class Environment; }

namespace dbi {

class Operator;
class RelationSchema;
class SPSegment;

namespace qopt {

class Predicate;
class TableAccessInfo;
class ColumnAccessInfo;

/// Interface for all optimizers
class Optimizer {
public:
   virtual ~Optimizer();
   virtual std::unique_ptr<Operator> optimize(const std::vector<TableAccessInfo>& relations, std::vector<std::unique_ptr<Predicate>>& predicates, std::set<ColumnAccessInfo>& projections) = 0;
};

}

}

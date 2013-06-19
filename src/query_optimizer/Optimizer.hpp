#pragma once

#include <vector>
#include <memory>
#include <string>

namespace harriet { class Expression; class Environment; }

namespace dbi {

class Operator;
class RelationSchema;
class SPSegment;

namespace qopt {

class Predicate;
class TableAccessInfo;

/// Interface for all optimizers
class Optimizer {
public:
   virtual ~Optimizer();
   virtual std::unique_ptr<Operator> optimize(const std::vector<TableAccessInfo>& relations, std::vector<std::unique_ptr<Predicate>>& predicates) = 0;
};

}

}

#pragma once

#include "Optimizer.hpp"
#include "TableAccessInfo.hpp"
#include <vector>
#include <memory>
#include <string>

namespace dbi {

namespace qopt {

/// This optimizer builds the cross product of all tables and then applies the predicates to select the right tuples.
/// Very fast optimizer but very slow query speed. Only use for _VERY_ small inputs. (Manly used to establish a performance baseline)
class DummyOptimizer : public Optimizer {
public:
   virtual std::unique_ptr<Operator> optimize(const std::vector<TableAccessInfo>& relations, std::vector<std::unique_ptr<Predicate>>& predicates);
};

}

}

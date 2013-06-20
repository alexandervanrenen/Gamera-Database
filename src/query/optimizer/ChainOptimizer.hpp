#pragma once

#include "Optimizer.hpp"
#include "query/util/TableAccessInfo.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace dbi {

namespace qopt {

class AccessTree;

/// This algorithm just joins tables which are connected with a predicate first.
/// Reasonable optimization sped and fast for common query types. Use this for big query where dynamic programming would be to slow or is not implemented.
/// WARNING: This algorithm assumes that each predicate refers to at least one table (handled by an assert)
class ChainOptimizer : public Optimizer {
public:
   ChainOptimizer(std::vector<harriet::Value>& globalRegister);
   virtual ~ChainOptimizer();
   virtual std::unique_ptr<Operator> optimize(const std::vector<TableAccessInfo>& relations, std::vector<std::unique_ptr<Predicate>>& predicates, std::set<ColumnAccessInfo>& projections);

private:
   std::unique_ptr<AccessTree> createAccessTree(const std::vector<TableAccessInfo>& relations, std::vector<std::unique_ptr<Predicate>>& predicates) const;
   std::vector<harriet::Value>& globalRegister;
};

}

}

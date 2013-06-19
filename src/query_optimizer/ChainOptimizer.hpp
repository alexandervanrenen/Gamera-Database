#pragma once

#include "Optimizer.hpp"
#include "query_util/TableAccessInfo.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace dbi {

namespace qopt {

class AccessTree;

/// This algorithm just joins tables which are connected with a predicate first.
/// Reasonable optimization sped and fast for common query types. Use this for big query where dynamic programming would be to slow or is not implemented.
/// WARNING: This algorithm assumes that each predicate refers to at least one table and that there are not two predicates referring to the same set of tables (each case is handled by an assert)
class ChainOptimizer : public Optimizer {
public:
   virtual ~ChainOptimizer();
   virtual std::unique_ptr<Operator> optimize(const std::vector<TableAccessInfo>& relations, std::vector<std::unique_ptr<Predicate>>& predicates);

private:
   struct Solution {
      ~Solution();
      float costs;
      std::unique_ptr<AccessTree> accessTree;
   };

   /// Maps a solution to the access tree (the vector is always ordered like the input)
   std::unordered_map<uint8_t, std::unordered_map<std::vector<TableAccessInfo*>, Solution>> solutionMap;
};

}

}

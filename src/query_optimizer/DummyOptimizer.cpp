#include "DummyOptimizer.hpp"
#include "util/Utility.hpp"
#include "operator/TableScanOperator.hpp"
#include "operator/SelectionOperator.hpp"
#include "operator/CrossProductOperator.hpp"
#include "harriet/Expression.hpp"
#include "Predicate.hpp"
#include <cstdint>

using namespace std;

namespace dbi {

namespace qopt {

unique_ptr<Operator> DummyOptimizer::optimize(const vector<TableAccessInfo>& relations, vector<std::unique_ptr<Predicate>>& predicates)
{
   // Cross-Product it with all other input relations
   unique_ptr<Operator> result;
   for(uint32_t i=0; i<relations.size(); i++) {
      auto nextLevel = util::make_unique<TableScanOperator>(*relations[i].segment, *relations[i].schema, relations[i].tableQualifier);
      if(i==0)
         result = move(nextLevel); else
         result = util::make_unique<CrossProductOperator>(move(nextLevel), move(result));
   }

   // Create selections
   for(auto& predicate : predicates)
      result = util::make_unique<SelectionOperator>(move(result), move(predicate->condition));
   return result;
}

}

}

#include "DummyOptimizer.hpp"
#include "harriet/Expression.hpp"
#include "query/operator/CrossProductOperator.hpp"
#include "query/operator/SelectionOperator.hpp"
#include "query/operator/TableScanOperator.hpp"
#include "query/util/Predicate.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "util/Utility.hpp"
#include <cstdint>

using namespace std;

namespace dbi {

namespace qopt {

unique_ptr<Operator> DummyOptimizer::optimize(const vector<TableAccessInfo>& relations, vector<std::unique_ptr<Predicate>>& predicates, set<ColumnAccessInfo>& projections)
{
   throw;
   // // Cross-Product it with all other input relations
   // unique_ptr<Operator> result;
   // for(uint32_t i=0; i<relations.size(); i++) {
   //    auto nextLevel = util::make_unique<TableScanOperator>(relations[i]);
   //    if(i==0)
   //       result = move(nextLevel); else
   //       result = util::make_unique<CrossProductOperator>(move(nextLevel), move(result));
   // }

   // // Create selections
   // for(auto& predicate : predicates)
   //    result = util::make_unique<SelectionOperator>(move(result), move(predicate));
   // return result;
}

}

}

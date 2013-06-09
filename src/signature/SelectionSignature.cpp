#include "SelectionSignature.hpp"
#include "schema/Common.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"

using namespace std;

namespace dbi {

SelectionSignature::SelectionSignature(const Signature& source, const Predicate& predicate)
{
   lhs = source.getAttributeIndex(predicate.lhs.tableIdentifier, predicate.lhs.columnIdentifier);
   rhs = source.getAttributeIndex(predicate.rhs.tableIdentifier, predicate.rhs.columnIdentifier);

   predicates.push_back(predicate);
}

bool SelectionSignature::fullfillsPredicates(const vector<unique_ptr<harriet::Value>>& tuple)
{
   return reinterpret_cast<harriet::BoolValue&>(*tuple[lhs]->computeEq(*tuple[rhs], harriet::Environment())).result;
}

}

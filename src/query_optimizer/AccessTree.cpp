#include "AccessTree.hpp"
#include "TableAccessInfo.hpp"
#include "TableAccessInfo.hpp"
#include "Predicate.hpp"
#include "harriet/Expression.hpp"
#include "operator/Operator.hpp"
#include "operator/TableScanOperator.hpp"
#include "operator/SelectionOperator.hpp"
#include "operator/CrossProductOperator.hpp"
#include "util/Utility.hpp"
#include <cassert>

using namespace std;

namespace dbi {

namespace qopt {

AccessTree::~AccessTree()
{
}

AccessTree::AccessTree()
{
}

Leafe::Leafe(unique_ptr<Predicate> p, uint32_t tableId, const TableAccessInfo& table)
: table(table)
{
   predicate=move(p);
   coveredRelations.insert(tableId);
}

Leafe::~Leafe()
{
}

unique_ptr<Operator> Leafe::toPlan() const
{
   unique_ptr<Operator> result = util::make_unique<TableScanOperator>(*table.segment, *table.schema, table.tableQualifier);
   
   if(predicate != nullptr)
      result = util::make_unique<SelectionOperator>(move(result), move(predicate->condition));
   return result;
}

Node::Node(unique_ptr<Predicate> p, unique_ptr<AccessTree> l, unique_ptr<AccessTree> r)
: lhs(move(l))
, rhs(move(r))
{
   predicate=move(p);
   coveredRelations.insert(lhs->coveredRelations.begin(), lhs->coveredRelations.end());
   coveredRelations.insert(rhs->coveredRelations.begin(), rhs->coveredRelations.end());
}

Node::~Node()
{
}

unique_ptr<Operator> Node::toPlan() const
{
   unique_ptr<Operator> result = util::make_unique<CrossProductOperator>(lhs->toPlan(), rhs->toPlan());
   if(predicate != nullptr)
      result = util::make_unique<SelectionOperator>(move(result), move(predicate->condition));
   return result;
}

}

}

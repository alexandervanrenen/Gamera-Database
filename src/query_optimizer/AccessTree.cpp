#include "AccessTree.hpp"
#include "query_util/TableAccessInfo.hpp"
#include "query_util/Predicate.hpp"
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

unique_ptr<Operator> Leafe::toPlan(const set<ColumnAccessInfo>& requiredColumns, vector<harriet::Value>& globalRegister)
{
   unique_ptr<Operator> result = util::make_unique<TableScanOperator>(table, requiredColumns, globalRegister);
   
   if(predicate != nullptr)
      result = util::make_unique<SelectionOperator>(move(result), move(predicate), globalRegister);
   return result;
}

set<ColumnAccessInfo> Leafe::getRequiredColumns() const
{
   set<ColumnAccessInfo> result;
   if(predicate != nullptr)
      for(auto& iter : predicate->columns)
         result.insert(iter);
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

unique_ptr<Operator> Node::toPlan(const set<ColumnAccessInfo>& requiredColumns, vector<harriet::Value>& globalRegister)
{
   auto lPlan = lhs->toPlan(requiredColumns, globalRegister);
   auto rPlan = rhs->toPlan(requiredColumns, globalRegister);
   unique_ptr<Operator> result = util::make_unique<CrossProductOperator>(move(lPlan), move(rPlan));
   if(predicate != nullptr)
      result = util::make_unique<SelectionOperator>(move(result), move(predicate), globalRegister);
   return result;
}

set<ColumnAccessInfo> Node::getRequiredColumns() const
{
   set<ColumnAccessInfo> result;

   // Self
   if(predicate != nullptr)
      for(auto& iter : predicate->columns)
         result.insert(iter);

   // Lhs
   auto lhsResult = lhs->getRequiredColumns();
   for(auto& iter : lhsResult)
      result.insert(iter);

   // Lhs
   auto rhsResult = rhs->getRequiredColumns();
   for(auto& iter : rhsResult)
      result.insert(iter);

   return result;
}

}

}

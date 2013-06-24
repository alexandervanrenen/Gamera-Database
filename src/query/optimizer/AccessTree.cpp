#include "AccessTree.hpp"
#include "harriet/Expression.hpp"
#include "query/operator/CrossProductOperator.hpp"
#include "query/operator/Operator.hpp"
#include "query/operator/SelectionOperator.hpp"
#include "query/operator/TableScanOperator.hpp"
#include "query/util/Predicate.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"
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

unique_ptr<Operator> Leafe::toPlan(const set<ColumnAccessInfo>& requiredColumns, vector<harriet::Value>& globalRegister, uint32_t& registerOffset)
{
   unique_ptr<Operator> result = util::make_unique<TableScanOperator>(table, requiredColumns, globalRegister, registerOffset);

   if(predicate != nullptr)
      result = util::make_unique<SelectionOperator>(move(result), move(predicate), globalRegister);
   return result;
}

set<ColumnAccessInfo> Leafe::getRequiredColumns() const
{
   set<ColumnAccessInfo> result;
   if(predicate != nullptr)
      for(auto& iter : predicate->requiredColumns)
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

unique_ptr<Operator> Node::toPlan(const set<ColumnAccessInfo>& requiredColumns, vector<harriet::Value>& globalRegister, uint32_t& registerOffset)
{
   auto lPlan = lhs->toPlan(requiredColumns, globalRegister, registerOffset);
   auto rPlan = rhs->toPlan(requiredColumns, globalRegister, registerOffset);
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
      for(auto& iter : predicate->requiredColumns)
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

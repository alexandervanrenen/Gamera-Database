#include "ChainOptimizer.hpp"
#include "AccessTree.hpp"
#include "harriet/Expression.hpp"
#include "query/operator/CrossProductOperator.hpp"
#include "query/operator/SelectionOperator.hpp"
#include "query/operator/TableScanOperator.hpp"
#include "query/operator/ZeroRecordOperator.hpp"
#include "query/util/Predicate.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "util/Utility.hpp"
#include <algorithm>
#include <cstdint>

using namespace std;

namespace dbi {

namespace qopt {

ChainOptimizer::ChainOptimizer(std::vector<harriet::Value>& globalRegister, const harriet::Environment& env)
: globalRegister(globalRegister)
, env(env)
{
}

ChainOptimizer::~ChainOptimizer()
{
}

unique_ptr<Operator> ChainOptimizer::optimize(const vector<TableAccessInfo>& relations, vector<unique_ptr<Predicate>>& predicates, set<ColumnAccessInfo>& projections)
{
   // Create a tree describing the order in which the tables are joined
   auto accessTree = createAccessTree(relations, predicates);

   if(accessTree) {
      // A normal access tree was produced
      set<ColumnAccessInfo> requiredColumns = accessTree->getRequiredColumns();
      for(auto& iter : projections)
         requiredColumns.insert(iter);
      for(auto& iter : requiredColumns)
         globalRegister.emplace_back(harriet::Value::createDefault(iter.columnSchema.type));
      uint32_t registerOffset = 0;
      auto plan = accessTree->toPlan(requiredColumns, globalRegister, registerOffset);
      assert(globalRegister.size() == registerOffset);
      return plan;
   } else {
      // Some brain dead idiot used 'false' in the where clause
      return util::make_unique<ZeroRecordOperator>(projections);
   }
}

// namespace {
//    void debug(vector<std::unique_ptr<AccessTree>>& workSet)
//    {
//       cout << "----" << endl;
//       for(auto& tree : workSet) {
//          cout << ">> tree:";
//          for(auto iter : tree->coveredRelations)
//             cout << " " << iter;
//          cout << endl;
//          if(tree->predicate != nullptr)
//             tree->predicate->dump(cout);
//          cout << endl;
//       }
//       cout << "----" << endl;
//    }
// }

unique_ptr<AccessTree> ChainOptimizer::createAccessTree(const vector<TableAccessInfo>& relations, vector<unique_ptr<Predicate>>& predicates) const
{
   // Handle predicates referring to no table
   for(auto iter = predicates.begin(); iter != predicates.end();) {
      if((*iter)->requiredColumns.empty()) {
         harriet::Value constantCondition = (*iter)->condition->evaluate(env);
         if(constantCondition.data.vbool) {
            // A true constant can be ignored
            iter = predicates.erase(iter);
         } else {
            // A false constant leads to an empty result
            return nullptr;
         }
      } else {
         iter++;
      }
   }

   // Find start points -- a predicate referring a single relation
   vector<std::unique_ptr<AccessTree>> workSet;
   for(uint32_t i=0; i<relations.size(); i++)
      workSet.push_back(util::make_unique<Leafe>(nullptr, i, relations[i]));

   // Iterative join all the basic table access
   bool keepOptimizing = true;
   while(keepOptimizing) {
      keepOptimizing = false;
//      debug(workSet);

      // Try to apply a predicate referring to a single tree
      for(auto predicate=predicates.begin(); predicate!=predicates.end(); predicate++) {
         set<uint32_t> requiredTrees = getRequiredTrees(**predicate, workSet);

         if(requiredTrees.size() == 1) {
            assert(workSet[*requiredTrees.begin()]->predicate == nullptr);
            workSet[*requiredTrees.begin()]->predicate = move(*predicate);
            predicates.erase(predicate);
            keepOptimizing = true;
            break;
         }
      }
      if(keepOptimizing)
         continue;

      // Try to join two trees with a predicate
      for(auto predicate=predicates.begin(); predicate!=predicates.end(); predicate++) {
         set<uint32_t> requiredTrees = getRequiredTrees(**predicate, workSet);
         assert(requiredTrees.size() > 1);

         if(requiredTrees.size() == 2) {
            unique_ptr<AccessTree> node = util::make_unique<Node>(move(*predicate), move(workSet[*requiredTrees.begin()]), move(workSet[*(++requiredTrees.begin())]));
            predicates.erase(predicate);
            workSet.erase(std::remove_if(workSet.begin(), workSet.end(), [](const unique_ptr<AccessTree>& a){return a==nullptr;}), workSet.end());
            workSet.push_back(move(node));
            keepOptimizing = true;
            break;
         }
      }
      if(keepOptimizing)
         continue;

      // Cross product with anything even remotely useful
      for(auto predicate=predicates.begin(); predicate!=predicates.end(); predicate++) {
         set<uint32_t> requiredTrees = getRequiredTrees(**predicate, workSet);
         assert(requiredTrees.size() > 2);

         unique_ptr<AccessTree> node = util::make_unique<Node>(nullptr, move(workSet[*requiredTrees.begin()]), move(workSet[*(++requiredTrees.begin())]));
         workSet.erase(std::remove_if(workSet.begin(), workSet.end(), [](const unique_ptr<AccessTree>& a){return a==nullptr;}), workSet.end());
         workSet.push_back(move(node));
         keepOptimizing = true;
         break;
      }
   }

   // Cross product the remaining relations
   while(workSet.size()>1) {
      auto lhs = move(workSet.back());
      workSet.pop_back();
      auto rhs = move(workSet.back());
      workSet.pop_back();
      unique_ptr<AccessTree> node = util::make_unique<Node>(nullptr, move(lhs), move(rhs));
      workSet.push_back(move(node));
   }

   assert(workSet.size() == 1);
   assert(predicates.size() == 0);
   return move(workSet[0]);
}

set<uint32_t> ChainOptimizer::getRequiredTrees(const Predicate& predicate, vector<unique_ptr<AccessTree> >& workSet) const
{
   set<uint32_t> requiredRelations = predicate.getRequiredTables();
   set<uint32_t> requiredTrees;
   for(uint32_t i=0; i<workSet.size(); i++) {
      for(auto suppliedTableId : workSet[i]->coveredRelations) {
         if(requiredRelations.count(suppliedTableId) == 1) {
            requiredTrees.insert(i);
            requiredRelations.erase(suppliedTableId);
         }
      }
   }
   return requiredTrees;
}

}

}

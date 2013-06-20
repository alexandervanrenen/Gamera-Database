#include "ChainOptimizer.hpp"
#include "query_util/TableAccessInfo.hpp"
#include "AccessTree.hpp"
#include "util/Utility.hpp"
#include "operator/TableScanOperator.hpp"
#include "operator/SelectionOperator.hpp"
#include "operator/CrossProductOperator.hpp"
#include "harriet/Expression.hpp"
#include "query_util/Predicate.hpp"
#include <cstdint>
#include <algorithm>

using namespace std;

namespace dbi {

namespace qopt {

ChainOptimizer::ChainOptimizer(std::vector<harriet::Value>& globalRegister)
: globalRegister(globalRegister)
{

}

ChainOptimizer::~ChainOptimizer()
{
}

unique_ptr<Operator> ChainOptimizer::optimize(const vector<TableAccessInfo>& relations, vector<unique_ptr<Predicate>>& predicates, set<ColumnAccessInfo>& projections)
{
   // Create a tree describing the order in which the tables are joined
   auto accessTree = createAccessTree(relations, predicates);

   set<ColumnAccessInfo> requiredColumns = accessTree->getRequiredColumns();
   for(auto& iter : projections)
      requiredColumns.insert(iter);
   return accessTree->toPlan(requiredColumns, globalRegister);
}

namespace {
   void debug(vector<std::unique_ptr<AccessTree>>& workSet)
   {
      cout << "----" << endl;
      for(auto& tree : workSet) {
         cout << ">> tree:";
         for(auto iter : tree->coveredRelations)
            cout << " " << iter;
         cout << endl;
         if(tree->predicate != nullptr)
            tree->predicate->dump(cout);
         cout << endl;
      }
      cout << "----" << endl;
   }
}

unique_ptr<AccessTree> ChainOptimizer::createAccessTree(const vector<TableAccessInfo>& relations, vector<unique_ptr<Predicate>>& predicates) const
{
   // Check that there is no predicate referring to no table
   assert(none_of(predicates.begin(), predicates.end(), [](unique_ptr<Predicate>& p){return p->tables.empty();}));

   // Find start points -- a predicate referring a single relation
   vector<std::unique_ptr<AccessTree>> workSet;
   for(uint32_t i=0; i<relations.size(); i++)
      workSet.push_back(util::make_unique<Leafe>(nullptr, i, relations[i]));

   // Iterative join all the basic table access
   bool changed = true;
   while(changed) {
      changed = false;
//      debug(workSet);

      // Try to apply a predicate referring to a single tree
      for(auto predicate=predicates.begin(); predicate!=predicates.end(); predicate++) {
         set<uint32_t> requiredRelations((*predicate)->tables.begin(), (*predicate)->tables.end());
         set<uint32_t> requiredTrees;
         for(uint32_t i=0; i<workSet.size(); i++) {
            for(auto suppliedTableId : workSet[i]->coveredRelations) {
               if(requiredRelations.count(suppliedTableId) == 1) {
                  requiredTrees.insert(i);
                  requiredRelations.erase(suppliedTableId);
               }
            }
         }

         if(requiredTrees.size() == 1) {
            assert(workSet[*requiredTrees.begin()]->predicate == nullptr);
            workSet[*requiredTrees.begin()]->predicate = move(*predicate);
            predicates.erase(predicate);
            changed = true;
            break;
         }
      }
      if(changed)
         continue;

      // Try to join two trees with a predicate
      for(auto predicate=predicates.begin(); predicate!=predicates.end(); predicate++) {
         set<uint32_t> requiredRelations((*predicate)->tables.begin(), (*predicate)->tables.end());
         set<uint32_t> requiredTrees;
         for(uint32_t i=0; i<workSet.size(); i++) {
            for(auto suppliedTableId : workSet[i]->coveredRelations) {
               if(requiredRelations.count(suppliedTableId) == 1) {
                  requiredTrees.insert(i);
                  requiredRelations.erase(suppliedTableId);
               }
            }
         }
         assert(requiredTrees.size() > 1);

         if(requiredTrees.size() == 2) {
            unique_ptr<AccessTree> node = util::make_unique<Node>(move(*predicate), move(workSet[*requiredTrees.begin()]), move(workSet[*(++requiredTrees.begin())]));
            predicates.erase(predicate);
            workSet.erase(std::remove_if(workSet.begin(), workSet.end(), [](const unique_ptr<AccessTree>& a){return a==nullptr;}), workSet.end());
            workSet.push_back(move(node));
            changed = true;
            break;
         }
      }
      if(changed)
         continue;

      // Cross product with anything even remotely useful
      for(auto predicate=predicates.begin(); predicate!=predicates.end(); predicate++) {
         set<uint32_t> requiredRelations((*predicate)->tables.begin(), (*predicate)->tables.end());
         set<uint32_t> requiredTrees;
         for(uint32_t i=0; i<workSet.size(); i++) {
            for(auto suppliedTableId : workSet[i]->coveredRelations) {
               if(requiredRelations.count(suppliedTableId) == 1) {
                  requiredTrees.insert(i);
                  requiredRelations.erase(suppliedTableId);
               }
            }
         }
         assert(requiredTrees.size() > 2);

         unique_ptr<AccessTree> node = util::make_unique<Node>(nullptr, move(workSet[*requiredTrees.begin()]), move(workSet[*(++requiredTrees.begin())]));
         workSet.erase(std::remove_if(workSet.begin(), workSet.end(), [](const unique_ptr<AccessTree>& a){return a==nullptr;}), workSet.end());
         workSet.push_back(move(node));
         changed = true;
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

}

}

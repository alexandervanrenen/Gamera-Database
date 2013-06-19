#include "PredicateGenerator.hpp"
#include "TableAccessInfo.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "query_parser/Common.hpp"
#include "util/Utility.hpp"
#include "query_util/ColumnResolver.hpp"
#include <iostream>

using namespace std;

namespace dbi {

namespace qopt {

PredicateGenerator::PredicateGenerator(const harriet::Environment& env)
: env(env)
{
}

// All conditions and all accessed tables
vector<unique_ptr<Predicate>> PredicateGenerator::createPredicates(vector<unique_ptr<harriet::Expression>>& conditions, const vector<TableAccessInfo>& tableAccessVec) const
{
   // 1: generate all predicates
   vector<unique_ptr<Predicate>> predicates;
   predicates.reserve(conditions.size());
   for(auto& expression: conditions) {
      predicates.push_back(createPredicate(move(expression), tableAccessVec));
   }
   // 2: check all predicates if tables match
   for(auto outerIter = predicates.begin(); outerIter != predicates.end(); outerIter++) {    
      for(auto innerIter = outerIter + 1; innerIter != predicates.end();) {
         // If so merge condition expression and drop one predicate
         if((*outerIter)->tables == (*innerIter)->tables) {
            (*outerIter)->condition = util::make_unique<harriet::AndOperator>(move((*outerIter)->condition), move((*innerIter)->condition));
            innerIter = predicates.erase(innerIter);
         } else 
            innerIter++;
      }
   }
   return predicates;
}

unique_ptr<Predicate> PredicateGenerator::createPredicate(unique_ptr<harriet::Expression> condition, const vector<TableAccessInfo>& tableAccessVec) const
{
   // Create predicate
   auto predicate = util::make_unique<Predicate>();
   predicate->condition = move(condition);

   // First -- Resolve all variables in the condition and add to the predicate
   {
      ColumnResolver columnResolver(env);
      vector<const harriet::Variable*> freeVariables = predicate->condition->getAllVariables();
      for(auto variable : freeVariables) {
         ColumnResolver::Result result = columnResolver.resolveSelection(variable->getIdentifier(), tableAccessVec);
         if(result.has()) {
            predicate->columns.push_back(result.get());
            predicate->tables.insert(predicate->columns.back().tableIndex);
         }
      }
   }

   // Second -- Check type of condition
   {
      harriet::Environment env;
      for(auto& iter : predicate->columns)
         env.add(iter.scriptName, harriet::Value::createDefault(iter.columnSchema.type));
      if(predicate->condition->evaluate(env).type.type != harriet::VariableType::Type::TBool) {
         ostringstream os;
         predicate->condition->print(os);
         throw harriet::Exception{"Result type of: '" + os.str() + "' is not bool."};
      }
   }

   return predicate;
}

}

}

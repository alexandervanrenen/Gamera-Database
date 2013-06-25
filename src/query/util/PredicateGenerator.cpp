#include "PredicateGenerator.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "query/parser/Common.hpp"
#include "query/util/ColumnResolver.hpp"
#include "TableAccessInfo.hpp"
#include "util/Utility.hpp"
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
         if((*outerIter)->getRequiredTables() == (*innerIter)->getRequiredTables()) {
            (*outerIter)->condition = harriet::Expression::createBinaryExpression(harriet::ExpressionType::TAndOperator, move((*outerIter)->condition), move((*innerIter)->condition));
            for(auto& iter : (*innerIter)->requiredColumns)
               (*outerIter)->requiredColumns.insert(iter);
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
      vector<string*> freeVariables = predicate->condition->getAllVariableNames();
      for(auto variableName : freeVariables) {
         ColumnResolver::Result result = columnResolver.resolveColumnReference(ColumnReference(*variableName), tableAccessVec);
         if(result.has()) {
            predicate->requiredColumns.insert(result.get());
            *variableName = result.get().columnReference.str();
         }
      }
   }

   // Second -- Check type of condition
   {
      harriet::Environment env;
      for(auto& iter : predicate->requiredColumns)
         if(!env.isInLocalScope(iter.columnReference.str()))
            env.add(iter.columnReference.str(), harriet::Value::createDefault(iter.columnSchema.type));

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

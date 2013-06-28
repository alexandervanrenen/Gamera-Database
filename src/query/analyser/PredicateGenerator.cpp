#include "PredicateGenerator.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "query/parser/Common.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "util/Utility.hpp"
#include "query/util/Predicate.hpp"
#include "query/util/GlobalRegister.hpp"
#include <iostream>

using namespace std;

namespace dbi {

namespace qgen {

PredicateGenerator::PredicateGenerator(harriet::Environment& environment, qopt::GlobalRegister& globalRegister)
: environment(environment)
, globalRegister(globalRegister)
{
}

// All conditions and all accessed tables
vector<unique_ptr<qopt::Predicate>> PredicateGenerator::createPredicates(vector<unique_ptr<harriet::Expression>>& conditions) const
{
   // 1: generate all predicates
   vector<unique_ptr<qopt::Predicate>> predicates;
   predicates.reserve(conditions.size());
   for(auto& expression: conditions)
      predicates.push_back(createPredicate(move(expression)));

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

unique_ptr<qopt::Predicate> PredicateGenerator::createPredicate(unique_ptr<harriet::Expression> condition) const
{
   // Create predicate
   auto predicate = util::make_unique<qopt::Predicate>(move(condition));

   // First -- Resolve all variables in the condition and add to the predicate
   {
      vector<string*> freeVariables = predicate->condition->getAllVariableNames();
      for(auto variableName : freeVariables) {
         if(globalRegister.hasColumn(*variableName)) {
            const qopt::RegisterSlotInfo& slot = globalRegister.getSlotInfo(globalRegister.getColumnIndex(*variableName));
            assert(slot.column != nullptr);
            predicate->requiredColumns.insert(*slot.column);
         }
      }
   }

   // Second -- Check type of condition
   {
      harriet::Environment env(&environment);
      for(auto& iter : predicate->requiredColumns)
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

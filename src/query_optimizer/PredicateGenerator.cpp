#include "PredicateGenerator.hpp"
#include "TableAccessInfo.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "query_parser/Common.hpp"
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
      vector<const harriet::Variable*> freeVariables = predicate->condition->getAllVariables();
      for(auto variable : freeVariables) {
         // Find table of the variable
         int16_t tableId = -1;
         ColumnReference ref(variable->getIdentifier());
         for(uint32_t id=0; id<tableAccessVec.size(); id++) {
            // Check if variable is a column in this table
            bool refersToTable = false;
            if(ref.tableQualifier.size()==0 || ref.tableQualifier==tableAccessVec[id].tableQualifier)
               refersToTable = tableAccessVec[id].schema->getAttribute(ref.columnName) != nullptr;

            // If so we found the table we are searching for
            if(refersToTable) {
               if(tableId == -1)
                  tableId = id; else
                  throw harriet::Exception{"ambiguous identifier '" + ref.tableQualifier + "." + ref.columnName + "', candidates: '" + tableAccessVec[tableId].tableQualifier + "." + ref.columnName + "' or '" + tableAccessVec[id].tableQualifier + "." + ref.columnName + "'"};
            }
         }
         if(tableId == -1)
            throw harriet::Exception{"unknown identifier: '" + variable->getIdentifier() + "'"};

         // Add to the list =)
         predicate->columns.push_back(ColumnAccessInfo(variable->getIdentifier(), *tableAccessVec[tableId].schema->getAttribute(ref.columnName)));
         predicate->tables.insert(tableId);
      }
   }

   // Second -- Check type of condition
   {
      harriet::Environment env;
      for(auto& iter : predicate->columns)
         env.add(iter.scriptName, harriet::Value::createDefault(iter.attributeSchema.type));
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

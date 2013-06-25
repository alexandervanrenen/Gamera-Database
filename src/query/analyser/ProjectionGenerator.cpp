#include "ProjectionGenerator.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "query/parser/Common.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "util/Utility.hpp"
#include "query/util/Projection.hpp"
#include "query/util/GlobalRegister.hpp"

using namespace std;

namespace dbi {

namespace qgen {

ProjectionGenerator::ProjectionGenerator(harriet::Environment& environment, qopt::GlobalRegister& globalRegister)
: environment(environment)
, globalRegister(globalRegister)
{
}

vector<unique_ptr<qopt::Projection>> ProjectionGenerator::createProjections(vector<pair<string, unique_ptr<harriet::Expression>>>& expressions) const
{
   vector<unique_ptr<qopt::Projection>> result;
   result.reserve(expressions.size());
   for(auto& expression : expressions)
      result.push_back(createProjection(move(expression)));
   return result;
}

unique_ptr<qopt::Projection> ProjectionGenerator::createProjection(pair<string, unique_ptr<harriet::Expression>> expression) const
{
   // Create projection
   auto projection = util::make_unique<qopt::Projection>(move(expression.second));

   // First -- Resolve all variables in the condition and add to the projection
   {
      vector<string*> freeVariables = projection->expression->getAllVariableNames();
      for(auto variableName : freeVariables) {
         if(globalRegister.hasColumn(*variableName)) {
            const qopt::RegisterSlotInfo& slot = globalRegister.getSlotInfo(globalRegister.getColumnIndex(*variableName));
            assert(slot.column != nullptr);
            projection->requiredColumns.insert(*slot.column);
         }
      }
   }

   // Second -- Infer the name of the resulting value
   {
      if(expression.first.size() != 0) {
         // User provided name
         projection->alias = expression.first;
      } else {
         // No name provided. Infer it if it is a simple column.
         if(projection->expression->getExpressionType() == harriet::ExpressionType::TVariable)
            projection->alias = projection->expression->identifier; else
            throw harriet::Exception("No name provided for projection '" + projection->prettyExpression + "'.");
      }
   }

   // Third -- Check types of the expression
   harriet::VariableType type;
   {
      harriet::Environment env(&environment);
      for(auto& iter : projection->requiredColumns)
         env.add(iter.columnReference.str(), harriet::Value::createDefault(iter.columnSchema.type));
      type = projection->expression->evaluate(env).type;
   }

   // Fourth -- Calculate target of register slot (if trivial no new slot is needed)
   {
      if(projection->expression->getExpressionType() == harriet::ExpressionType::TVariable) {
         if(globalRegister.hasColumn(projection->expression->identifier)) {
            // Depends on an already present value => reuse slot
            projection->resultRegisterSlot = globalRegister.getColumnIndex(projection->expression->identifier);
         } else {
            // A new independent value => new slot
            globalRegister.addProjectedColumn(projection->alias, type);
            projection->resultRegisterSlot = globalRegister.getColumnIndex(projection->alias);
         }
      } else {
         // Calculation => new slot
         globalRegister.addProjectedColumn(projection->alias, type);
         projection->resultRegisterSlot = globalRegister.getColumnIndex(projection->alias);
      }
   }

   return projection;
}

}

}


#include "ExpressionOptimizer.hpp"
#include "harriet/Expression.hpp"
#include "query/util/GlobalRegister.hpp"

using namespace std;

namespace dbi {

namespace qgen {

unique_ptr<harriet::Expression> ExpressionOptimizer::fullOptimization(const harriet::Expression& expression, qopt::GlobalRegister& globalRegister, harriet::Environment& environment)
{
   harriet::Expression* ptr = expression.createCopy().release();
   substitute(&ptr, globalRegister);
   substitute(&ptr, environment);
   optimize(&ptr);
   return unique_ptr<harriet::Expression>(ptr);
}

void ExpressionOptimizer::substitute(harriet::Expression** expression, qopt::GlobalRegister& globalRegister) const
{
   vector<harriet::Expression**> freeVariables = (*expression)->getAllVariables(expression);
   for(auto iter : freeVariables) {
      if(globalRegister.hasColumn((*iter)->identifier)) {
         uint32_t columnIndex = globalRegister.getColumnIndex((*iter)->identifier);
         delete *iter;
         *iter = harriet::Expression::createValueReferenceExpression(&globalRegister.getSlotValue(columnIndex)).release();
      }
   }
}

void ExpressionOptimizer::substitute(harriet::Expression**, harriet::Environment&) const
{
}

void ExpressionOptimizer::optimize(harriet::Expression**) const
{
}

}

}

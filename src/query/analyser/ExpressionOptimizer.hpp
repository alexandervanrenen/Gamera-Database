#include <memory>

namespace harriet { class Expression; class Environment; }

namespace dbi {

namespace qopt { class GlobalRegister; }

namespace qgen {

class ExpressionOptimizer {
public:
   /// Run all optimization steps in the correct order and returns result in a new Expression (NOTE: this expression is only valid als long as the register and the environment does not change)
   std::unique_ptr<harriet::Expression> fullOptimization(const harriet::Expression& expression, qopt::GlobalRegister& globalRegister, harriet::Environment& environment);

private:
   /// Using direct pointers into the register
   void substitute(harriet::Expression** expression, qopt::GlobalRegister& globalRegister) const;
   /// Substituting values for each variable in the environment (WARNING: will replace all variables found (column identifier)
   void substitute(harriet::Expression** expression, harriet::Environment& environment) const;
   /// Optimizes constant parts of the expression
   void optimize(harriet::Expression** expression) const;
};

}

}

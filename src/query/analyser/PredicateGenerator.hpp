#pragma once

#include <memory>
#include <vector>

namespace harriet { class Environment; class Expression; }

namespace dbi {

namespace qopt { class Predicate; class TableAccessInfo; class GlobalRegister; }

namespace qgen {

class PredicateGenerator {
public:
   PredicateGenerator(harriet::Environment& environment, qopt::GlobalRegister& globalRegister);
   
   std::vector<std::unique_ptr<qopt::Predicate>> createPredicates(std::vector<std::unique_ptr<harriet::Expression>>& conditions) const;

private:
   harriet::Environment& environment;
   qopt::GlobalRegister& globalRegister;

   std::unique_ptr<qopt::Predicate> createPredicate(std::unique_ptr<harriet::Expression> condition) const;
};

}

}

#pragma once

#include "Predicate.hpp"
#include <memory>
#include <vector>

namespace harriet { class Environment; class Expression; }

namespace dbi {

namespace qopt {

class TableAccessInfo;

class PredicateGenerator {
public:
   PredicateGenerator(const harriet::Environment& env);
   
   std::vector<std::unique_ptr<Predicate>> createPredicates(std::vector<std::unique_ptr<harriet::Expression>>& conditions, const std::vector<TableAccessInfo>& tableAccessVec) const;

private:
   const harriet::Environment& env;
   
   std::unique_ptr<Predicate> createPredicate(std::unique_ptr<harriet::Expression> condition, const std::vector<TableAccessInfo>& tableAccessVec) const;
};

}

}

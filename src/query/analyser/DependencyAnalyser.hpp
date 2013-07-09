#pragma once

#include <set>
#include <string>
#include <vector>
#include <memory>

namespace harriet { class Expression; class Environment; }

namespace dbi {

class ColumnReference;

namespace qopt { class TableAccessInfo; class GlobalRegister; class ColumnAccessInfo; }

namespace qgen {

class DependencyAnalyser {
public:
   DependencyAnalyser(const harriet::Environment& environment, const std::vector<qopt::TableAccessInfo>& tableAccessInfos);

   /// Finds all columns accesses in the projections and predicates and creates the global register from them
   std::unique_ptr<qopt::GlobalRegister> createGlobalRegister(std::vector<std::pair<std::string, std::unique_ptr<harriet::Expression>>>& selectors, std::vector<std::unique_ptr<harriet::Expression>>& conditions, std::vector<std::string>& orderBy);

private:
   // Resolves a single column reference and adds it to the required columns
   std::string resolveColumnReference(const ColumnReference& columnReference);

   /// Data
   const harriet::Environment& environment;
   const std::vector<qopt::TableAccessInfo>& tableAccessInfos;
   std::set<qopt::ColumnAccessInfo> requiredColumns;
};

}

}

#pragma once

#include "ColumnAccessInfo.hpp"
#include "harriet/Expression.hpp"
#include "TableAccessInfo.hpp"
#include <memory>
#include <vector>

namespace harriet { class Environment; class Expression; }

namespace dbi {

class ColumnReference;

namespace qopt {

class ColumnResolver {
public:
   ColumnResolver(const harriet::Environment& env);

   struct Result {
      Result(std::unique_ptr<ColumnAccessInfo> column) : column(move(column)) {}
      bool has() const { return column!=nullptr; }
      ColumnAccessInfo get() const { return *column; }
   private:
      std::unique_ptr<ColumnAccessInfo> column;
   };

   Result resolveSelection(const ColumnReference& column, const std::vector<TableAccessInfo>& tableAccessVec) const;
   ColumnAccessInfo resolveProjection(const ColumnReference& column, const std::vector<TableAccessInfo>& tableAccessVec) const;

private:
   const harriet::Environment& env;

   static std::pair<uint32_t, const ColumnSchema*> tryFindColumn(const ColumnReference& ref, const std::vector<TableAccessInfo>& tableAccessVec);
};

}

}

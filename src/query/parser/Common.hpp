#pragma once

#include "harriet/ScriptLanguage.hpp"
#include <memory>
#include <string>

namespace harriet {
   class Value;
}

namespace dbi {

/// In "select s.name from Students s;" the term "s.name" is a ColumnIdentifier.
struct ColumnReference {
   ColumnReference() { }
   ColumnReference(const std::string& str) {size_t splitPos=str.find('.'); tableQualifier=str.substr(0, splitPos); columnName=(splitPos==std::string::npos?"":str.substr(splitPos+1, str.size())); }
   ColumnReference(const std::string& tableQalifier, const std::string& columnName) : tableQualifier(tableQalifier), columnName(columnName) { }

   std::string tableQualifier;
   std::string columnName;
};

/// In "select s.name from Students s;" the term "Students s" is a TableAccess.
struct TableReference {
   std::string tableName;
   std::string tableQualifier;
};

// In "create table Students(id integer not null)" the term "id integer not null" as an AttributeDeclaration.
struct AttributeDeclaration {
   std::string name;
   harriet::VariableType type;
   bool notNull;
};

}

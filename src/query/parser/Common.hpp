#pragma once

#include "harriet/ScriptLanguage.hpp"
#include <memory>
#include <string>
#include <cassert>

namespace harriet {
   class Value;
}

namespace dbi {

/// In "select s.name from Students s;" the term "s.name" is a ColumnIdentifier. The exact way the expression appeared in the script.
struct ColumnReference {
   ColumnReference() { }
   ColumnReference(std::string&& tableQalifier, std::string&& columnName) : tableQualifier(move(tableQalifier)), columnName(move(columnName)) { }
   ColumnReference(const std::string& input) {size_t splitPos=input.find('.'); if(splitPos==std::string::npos) {columnName=input;} else {tableQualifier=input.substr(0, splitPos); assert(splitPos<input.size()); columnName=input.substr(splitPos+1, input.size());} }
   ColumnReference(const std::string& tableQalifier, const std::string& columnName) : tableQualifier(tableQalifier), columnName(columnName) { }

   std::string tableQualifier;
   std::string columnName;

   const std::string str() const {return (tableQualifier.size()==0?"":tableQualifier+".") + columnName;}
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

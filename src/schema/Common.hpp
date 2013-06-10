#pragma once

#include <string>
#include <memory>

namespace harriet {
   class Value;
}

namespace dbi {

/// In "select s.name from Students s;" the term "s.name" is a ColumnIdentifier.
struct ColumnReference {
   std::string tableQalifier;
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
   std::string type;
   bool notNull;
};

}

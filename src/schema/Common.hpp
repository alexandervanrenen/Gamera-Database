#pragma once

#include <string>

namespace dbi {

/// In "select s.name from Students s;" the term "s.name" is a ColumnIdentifier.
struct ColumnIdentifier {
   std::string tableIdentifier;
   std::string columnIdentifier;
};

/// In "select s.name from Students s;" the term "Students s" is a TableAccess.
struct TableAccess {
   std::string tableIdentifier;
   std::string alias;
};

// In "create table Students(id integer not null)" the term "id integer not null" as an AttributeDeclaration.
struct AttributeDeclaration {
   std::string name;
   std::string type;
   bool notNull;
};

/// In "select * from Students where id = 5;" the term "id = 5" is a Predicate.
struct Predicate {
   ColumnIdentifier lhs;
   std::string op;
   ColumnIdentifier rhs;
};

}

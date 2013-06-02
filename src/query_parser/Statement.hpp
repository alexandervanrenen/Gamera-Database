#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

namespace dbi {

namespace script {

/// In "select s.name from Students s;" the term "s.name" is a ColumnIdentifier.
struct ColumnIdentifier { std::string tableIdentifier; std::string columnIdentifier; };

/// In "select s.name from Students s;" the term "Students s" is a TableAccess.
struct TableAccess { std::string tableIdentifier; std::string alias; };

// In "create table Students(id integer not null)" the term "id integer not null" as an AttributeDeclaration.
struct AttributeDeclaration { std::string name; std::string type; bool notNull; void dump() {std::cout << name << " " << type << " " << notNull << std::endl;} };

struct Statement {
   virtual void dump() = 0;
};

struct SelectStatement : public Statement {

   SelectStatement(std::vector<ColumnIdentifier> selectors, std::vector<TableAccess> sources) : selectors(selectors), sources(sources) {}

   std::vector<ColumnIdentifier> selectors;
   std::vector<TableAccess> sources;

   virtual void dump() {
      std::cout << "---------" << std::endl;
      std::cout << "select " << std::endl;
      for(auto& iter : selectors)
         std::cout << "    " << iter.tableIdentifier << " " << iter.columnIdentifier << std::endl;
      std::cout << "from " << std::endl;
      for(auto& iter : sources)
         std::cout << "    " << iter.tableIdentifier << " " << iter.alias << std::endl;
      std::cout << "---------" << std::endl;
   }
};

struct CreateStatement : public Statement {

   CreateStatement(const std::string& name, std::vector<AttributeDeclaration>& attributes) : name(name), attributes(attributes) {}

   std::string name;
   std::vector<AttributeDeclaration> attributes;
   // std::vector<unsigned> primaryKey;

   virtual void dump() {
      std::cout << "---------" << std::endl;
      std::cout << "create " << name << std::endl;
      for(auto& iter : attributes)
         iter.dump();
      std::cout << "---------" << std::endl;
   }
};

}

}

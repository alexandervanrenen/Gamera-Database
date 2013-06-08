#pragma once

#include "Visitor.hpp"
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

namespace harriet {
   class Value;
}

namespace dbi {

namespace script {

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

/// 
struct Statement {
   virtual ~Statement();
   virtual void acceptVisitor(Visitor& visitor) = 0;

   bool isLocal() const;
   bool isGlobal() const;

   enum struct Type : uint8_t {kSelectStatement, kCreateTableStatement, kInsertStatement, kBlockStatement, kRootStatement};
   virtual Statement::Type getType() const = 0;
};

/// 
struct SelectStatement : public Statement {

   SelectStatement(std::vector<ColumnIdentifier>&& selectors, std::vector<TableAccess>&& sources);

   std::vector<ColumnIdentifier> selectors;
   std::vector<TableAccess> sources;

   virtual Statement::Type getType() const {return Statement::Type::kSelectStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct CreateTableStatement : public Statement {

   CreateTableStatement(const std::string& name, std::vector<AttributeDeclaration>&& attributes);

   std::string name;
   std::vector<AttributeDeclaration> attributes;
   // std::vector<unsigned> primaryKey;

   virtual Statement::Type getType() const {return Statement::Type::kCreateTableStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct InsertStatement : public Statement {

   InsertStatement(const std::string& tableName, std::vector<std::unique_ptr<harriet::Value>>&& values);

   std::string tableName;
   std::vector<std::unique_ptr<harriet::Value>> values;

   virtual Statement::Type getType() const {return Statement::Type::kInsertStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct BlockStatement : public Statement {

   std::vector<std::unique_ptr<Statement>> statements;

   BlockStatement(std::vector<std::unique_ptr<Statement>> statements);

   virtual Statement::Type getType() const {return Statement::Type::kBlockStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct RootStatement : public Statement {

   std::vector<std::unique_ptr<Statement>> statements;

   RootStatement(std::vector<std::unique_ptr<Statement>> statements);

   virtual Statement::Type getType() const {return Statement::Type::kRootStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

}

}

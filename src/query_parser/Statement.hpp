#pragma once

#include "Visitor.hpp"
#include "schema/Common.hpp"
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

namespace harriet {
   class Value;
   class Expression;
}

namespace dbi {

namespace script {

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

   SelectStatement(std::vector<ColumnReference>&& selectors, std::vector<TableReference>&& sources, std::vector<std::unique_ptr<harriet::Expression>>&& predicates);

   std::vector<ColumnReference> selections;
   std::vector<TableReference> sources;
   std::vector<std::unique_ptr<harriet::Expression>> predicates;

   virtual Statement::Type getType() const {return Statement::Type::kSelectStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct CreateTableStatement : public Statement {

   CreateTableStatement(const std::string& name, std::vector<AttributeDeclaration>&& attributes);

   std::string tableName;
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

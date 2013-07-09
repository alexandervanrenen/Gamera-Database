#pragma once

#include "Common.hpp"
#include "harriet/Value.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "Visitor.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace harriet {
   class Value;
   class Expression;
}

namespace dbi {

namespace qopt { class GlobalRegister; }

class RootOperator;
class PrintOperator;

namespace script {

/// 
struct Statement {
   virtual ~Statement();
   virtual void acceptVisitor(Visitor& visitor) = 0;

   bool isLocal() const;
   bool isGlobal() const;

   enum struct Type : uint8_t {kSelectStatement, kCreateTableStatement, kInsertStatement, kDropTableStatement, kBlockStatement, kRootStatement};
   virtual Statement::Type getType() const = 0;
};

/// 
struct SelectStatement : public Statement {

   SelectStatement(std::vector<std::pair<std::string, std::unique_ptr<harriet::Expression>>>&& selectors, std::vector<TableReference>&& sources, std::vector<std::unique_ptr<harriet::Expression>>&& conditions, std::vector<std::string>&& orderBy);
   ~SelectStatement();

   std::vector<std::pair<std::string, std::unique_ptr<harriet::Expression>>> projections;
   std::vector<TableReference> sources;
   std::vector<std::unique_ptr<harriet::Expression>> conditions;
   std::vector<std::string> orderBy;

   std::unique_ptr<PrintOperator> queryPlan;

   std::vector<qopt::TableAccessInfo> tableAccessVec; // Temporary here .. move to script
   std::unique_ptr<qopt::GlobalRegister> globalRegister; // Temporary here .. move to script

   virtual Statement::Type getType() const {return Statement::Type::kSelectStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct CreateTableStatement : public Statement {

   CreateTableStatement(std::string&& name, std::vector<AttributeDeclaration>&& attributes, std::vector<std::vector<std::string>>&& uniqueColumns);

   std::string tableName;
   std::vector<AttributeDeclaration> attributes;
   std::vector<std::vector<std::string>> uniqueColumns;

   virtual Statement::Type getType() const {return Statement::Type::kCreateTableStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct InsertStatement : public Statement {

   InsertStatement(std::string&& tableName, std::vector<harriet::Value>&& values);
   ~InsertStatement();

   std::string tableName;
   std::vector<harriet::Value> values;

   std::unique_ptr<RootOperator> queryPlan;
   std::unique_ptr<qopt::GlobalRegister> globalRegister; // Temporary here .. move to script

   virtual Statement::Type getType() const {return Statement::Type::kInsertStatement;}

   virtual void acceptVisitor(Visitor& visitor);
};

/// 
struct DropTableStatement : public Statement {

   DropTableStatement(std::string&& tableName);
   ~DropTableStatement();

   std::string tableName;

   virtual Statement::Type getType() const {return Statement::Type::kDropTableStatement;}

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

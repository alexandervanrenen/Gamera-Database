#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <vector>

namespace harriet { class Expression; class Value; }

namespace dbi {

class Operator;

namespace qopt {

class Predicate;
class TableAccessInfo;
class ColumnAccessInfo;

struct AccessTree {
   virtual ~AccessTree();

   virtual std::unique_ptr<Operator> toPlan(const std::set<ColumnAccessInfo>& requiredColumns, std::vector<harriet::Value>& globalRegister) = 0; // Destroys the tree, as it moves the predicates into the operator
   virtual std::set<ColumnAccessInfo> getRequiredColumns() const = 0;

   std::unique_ptr<Predicate> predicate;
   std::set<uint32_t> coveredRelations;
protected:
   AccessTree();
};

struct Leafe : public AccessTree {
   Leafe(std::unique_ptr<Predicate> p, uint32_t tableId, const TableAccessInfo& table);
   virtual ~Leafe();

   virtual std::unique_ptr<Operator> toPlan(const std::set<ColumnAccessInfo>& requiredColumns, std::vector<harriet::Value>& globalRegister);
   virtual std::set<ColumnAccessInfo> getRequiredColumns() const;

   const TableAccessInfo& table;
};

struct Node : AccessTree {
   Node(std::unique_ptr<Predicate> p, std::unique_ptr<AccessTree> l, std::unique_ptr<AccessTree> r);
   virtual ~Node();

   virtual std::unique_ptr<Operator> toPlan(const std::set<ColumnAccessInfo>& requiredColumns, std::vector<harriet::Value>& globalRegister);
   virtual std::set<ColumnAccessInfo> getRequiredColumns() const;

   std::unique_ptr<AccessTree> lhs;
   std::unique_ptr<AccessTree> rhs;
};

}

}

#pragma once

#include <vector>
#include <memory>
#include <set>
#include <cstdint>

namespace harriet { class Expression; }

namespace dbi {

class Operator;

namespace qopt {

class Predicate;
class TableAccessInfo;

struct AccessTree {
   virtual ~AccessTree();
   virtual std::unique_ptr<Operator> toPlan() const = 0;
   std::unique_ptr<Predicate> predicate;
   std::set<uint32_t> coveredRelations;
protected:
   AccessTree();
};

struct Leafe : public AccessTree {
   Leafe(std::unique_ptr<Predicate> p, uint32_t tableId, const TableAccessInfo& table);
   virtual ~Leafe();
   virtual std::unique_ptr<Operator> toPlan() const;
   const TableAccessInfo& table;
};

struct Node : AccessTree {
   Node(std::unique_ptr<Predicate> p, std::unique_ptr<AccessTree> l, std::unique_ptr<AccessTree> r);
   virtual ~Node();
   virtual std::unique_ptr<Operator> toPlan() const;

   std::unique_ptr<AccessTree> lhs;
   std::unique_ptr<AccessTree> rhs;
};

}

}

#pragma once

#include "RootOperator.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace harriet { class Value; }

namespace dbi {

class SPSegment;
class Operator;
class RelationSchema;

/// Insert the tuples provided into the given table -- this is a root operator (can not have a parent)
class InsertOperator : public RootOperator {
public:
   InsertOperator(std::unique_ptr<Operator> source, SPSegment& target, const RelationSchema& targetSchema, std::vector<harriet::Value>& globalRegister);
   virtual ~InsertOperator();
   virtual void dump(std::ostream& os) const;

   void checkTypes() const throw(harriet::Exception);

   void execute();

private:
   std::unique_ptr<Operator> source;
   SPSegment& target;
   const RelationSchema& targetSchema;
   std::vector<harriet::Value>& globalRegister;
};

}

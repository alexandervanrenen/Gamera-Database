#pragma once

#include "RootOperator.hpp"
#include "common/Config.hpp"
#include "OperatorState.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include "schema/RelationSchema.hpp"
#include "harriet/ScriptLanguage.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;
class Operator;

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

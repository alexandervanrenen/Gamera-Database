#pragma once

#include "common/Config.hpp"
#include "Operator.hpp"
#include "OperatorState.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include "schema/RelationSchema.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;
class Operator;

/// Insert the tuples provided into the given table -- this is a root operator (can not have a parent)
class InsertOperator {
public:
   InsertOperator(std::unique_ptr<Operator> source, SPSegment& target, const RelationSchema& targetSchema);
   virtual ~InsertOperator();

   void execute();

private:
   std::unique_ptr<Operator> source;
   SPSegment& target;
   const RelationSchema& targetSchema;
};

}

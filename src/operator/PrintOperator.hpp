#pragma once

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

/// Print the tuples supplied by the source to the given output stream
class PrintOperator {
public:
   PrintOperator(std::unique_ptr<Operator> source, std::ostream& out);
   virtual ~PrintOperator();

   void checkTypes() const throw(harriet::Exception);

   void execute();

private:
   std::unique_ptr<Operator> source;
   std::ostream& out;
};

}

#pragma once

#include "RootOperator.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace dbi {

class SPSegment;
class Operator;

/// Print the tuples supplied by the source to the given output stream
class PrintOperator : public RootOperator {
public:
   PrintOperator(std::unique_ptr<Operator> source, std::ostream& out, std::vector<harriet::Value>& globalRegister);
   virtual ~PrintOperator();

   void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os) const;

   void execute();

private:
   std::vector<harriet::Value>& globalRegister;
   std::unique_ptr<Operator> source;
   std::ostream& out;
};

}

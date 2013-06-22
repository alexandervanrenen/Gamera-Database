#pragma once

#include "RootOperator.hpp"
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

namespace dbi {

class SPSegment;
class Operator;

/// Print the tuples supplied by the source to the given output stream
class PrintOperator : public RootOperator {
public:
   PrintOperator(std::unique_ptr<Operator> source, std::vector<harriet::Value>& globalRegister);
   virtual ~PrintOperator();

   void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os) const;

   std::vector<std::vector<harriet::Value>>&& getResult();
   std::vector<std::string> getSuppliedColumns();
   std::chrono::nanoseconds getExecutionTime() const;

   void execute();

private:
   std::vector<harriet::Value>& globalRegister;
   std::unique_ptr<Operator> source;
   std::vector<std::vector<harriet::Value>> result;
   std::chrono::nanoseconds executionTime;
};

}

#pragma once

#include "RootOperator.hpp"
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

namespace dbi {

namespace qopt { class GlobalRegister; }

class SPSegment;
class ProjectionOperator;

/// Print the tuples supplied by the source to the given output stream
class PrintOperator : public RootOperator {
public:
   PrintOperator(std::unique_ptr<ProjectionOperator> source, qopt::GlobalRegister& globalRegister);
   virtual ~PrintOperator();

   void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os) const;

   std::vector<std::vector<harriet::Value>>&& getResult();
   std::vector<std::string> getSuppliedColumns();
   std::chrono::nanoseconds getExecutionTime() const;

   void execute();

private:
   qopt::GlobalRegister& globalRegister;
   std::unique_ptr<ProjectionOperator> source;
   std::vector<std::vector<harriet::Value>> result;
   std::chrono::nanoseconds executionTime;
};

}

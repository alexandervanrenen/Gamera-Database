#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace harriet { class VariableType; }

namespace dbi {

class IndexKey;
class ExternalSort;

namespace qopt { class Projection; class GlobalRegister; }

/// Logic: 1. Read complete input and write stream based to file. 2. Sort the file. 3. Read file sequentially.
class SortOperator : public Operator {
public:
   SortOperator(std::unique_ptr<Operator> source, const std::vector<std::string>& orderBy, qopt::GlobalRegister& globalRegister);
   virtual ~SortOperator();

   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   std::unique_ptr<Operator> source;
   OperatorState state;
   const std::vector<std::string> orderBy;
   qopt::GlobalRegister& globalRegister;
   std::unique_ptr<IndexKey> key;
   std::unique_ptr<ExternalSort> sorty;
   bool alreadySorted;
};

}

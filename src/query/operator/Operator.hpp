#pragma once

#include <ios>

namespace dbi {

class Signature;

namespace qopt { class ColumnAccessInfo; }

/// Just define interface for all non root operators
class Operator {
public:
   /// Which columns does the operator provide in which order ?
   virtual const Signature& getSignature() const = 0;
   /// Print the operator in a single line.
   virtual void dump(std::ostream& os, uint32_t lvl) const = 0;

   /// Prepare the operator for usage. e.g. allocate some buffers or set up iterators.
   virtual void open() = 0;
   /// Load the next tuple into the global register and return true. If no more tuples are left, the global register is not touched and false is returned.
   virtual bool next() = 0;
   /// Free up processing resources.
   virtual void close() = 0;

   virtual ~Operator();
};

}

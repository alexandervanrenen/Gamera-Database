#pragma once

#include "Signature.hpp"
#include <set>

namespace dbi {

namespace qopt { class ColumnAccessInfo; }

class ProjectionSignature : public Signature {
public:
   /// Create signature using the supplied signature of the underlying operator and the projection target
   ProjectionSignature(const std::vector<qopt::ColumnAccessInfo>& target);
   ~ProjectionSignature();

   void prepare(const Signature& source);

   std::set<qopt::ColumnAccessInfo> getRequiredColumns() const;

private:
   std::vector<uint32_t> projection;
   const std::vector<qopt::ColumnAccessInfo> target;
};

}

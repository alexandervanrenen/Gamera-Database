#pragma once

#include "Signature.hpp"

namespace dbi {

class ColumnReference;

class ProjectionSignature : public Signature {
public:
   /// Create signature using the supplied signature of the underlying operator and the projection target
   ProjectionSignature(const Signature& source, const std::vector<ColumnReference>& target);
   /// Result[0] = 4 means that the first element of the result tuple is found at the fifth position in the source tuple
   std::vector<uint32_t> getProjection() const;

private:
   std::vector<uint32_t> projection;
};

}

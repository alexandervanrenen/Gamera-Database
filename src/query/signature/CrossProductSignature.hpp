#pragma once

#include "Signature.hpp"

namespace dbi {

class CrossProductSignature : public Signature {
public:
   ///
   CrossProductSignature(const Signature& lhs, const Signature& rhs);
};

}

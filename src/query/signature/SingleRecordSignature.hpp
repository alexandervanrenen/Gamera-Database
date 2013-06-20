#pragma once

#include "Signature.hpp"

namespace dbi {

class SingleRecordSignature : public Signature {
public:
   // Deduce schema from an expression in a script. For the SingleRecordOperator
   SingleRecordSignature(const std::vector<harriet::Value>& values);
};

}

#pragma once

#include "Signature.hpp"

namespace dbi {

class RelationSchema;

class TableScanSignature : public Signature {
public:
   // Create named variables from a TableScanOperator
   TableScanSignature(const RelationSchema& relationSchema, const std::string& alias);
};

}

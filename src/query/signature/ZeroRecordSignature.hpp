#pragma once

#include "Signature.hpp"
#include <set>

namespace dbi {

namespace qopt { class ColumnAccessInfo; }

class ZeroRecordSignature : public Signature {
public:
   // Just forward the required schema.
   ZeroRecordSignature(std::set<qopt::ColumnAccessInfo>& projections);
};

}

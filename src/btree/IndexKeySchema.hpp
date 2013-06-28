#pragma once

#include <vector>
#include <cstdint>

namespace harriet { class VariableType; }

namespace dbi {

class IndexKeySchema {
public:
   IndexKeySchema(const std::vector<harriet::VariableType>& schema);

   const std::vector<harriet::VariableType> getSchema() const;

   uint64_t bytes() const;

private:
   const std::vector<harriet::VariableType> schema;
};

}

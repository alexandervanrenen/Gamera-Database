#pragma once

#include <vector>

namespace harriet { class VariableType; }

namespace dbi {

class IndexKeySchema {
public:
   IndexKeySchema(const std::vector<harriet::VariableType>& schema);

   const std::vector<harriet::VariableType> getSchema() const;

private:
   const std::vector<harriet::VariableType> schema;
};

}

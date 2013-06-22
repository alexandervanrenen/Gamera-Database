#pragma once

#include "harriet/VariableType.hpp"
#include <string>

namespace dbi {

struct ColumnSignature {
   std::string name;
   std::string alias;
   bool notNull;
   harriet::VariableType type;
   uint32_t index; // In global register
   uint32_t tableIndex; // Table id
};

}

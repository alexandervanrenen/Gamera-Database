#pragma once

#include "common/AttributeType.hpp"

namespace dbi {

AttributeType intToType(uint8_t value);

uint8_t typeToInt(AttributeType t);

}


#include "AttributeTypeConversion.hpp"

namespace dbi {

AttributeType intToType(uint8_t value) {
    switch (value) {
    case 10: return AttributeType::Integer;
    case 11: return AttributeType::Char;
    }
    return AttributeType::Invalid;
}

uint8_t typeToInt(AttributeType t) {
    switch (t) {
    case AttributeType::Invalid: return 1;
    case AttributeType::Null: return 0;
    case AttributeType::Integer: return 10;
    case AttributeType::Char: return 11;
    }
    return 0;
}

}

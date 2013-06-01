#include "Schema.hpp"

#include <sstream>

namespace dbi {
namespace sqlparser {

static std::string type(const Schema::Relation::Attribute& attr) {
   AttributeType type = attr.type;
   switch(type) {
      case AttributeType::Integer:
         return "Integer";
      /*case AttributeType::Numeric: {
         std::stringstream ss;
         ss << "Numeric(" << attr.len1 << ", " << attr.len2 << ")";
         return ss.str();
      }*/
      case AttributeType::Char: {
         std::stringstream ss;
         ss << "Char(" << attr.len << ")";
         return ss.str();
      }
    case AttributeType::Null:
    case AttributeType::Invalid:
        throw;
   }
   throw;
}

std::string Schema::toString() const {
   std::stringstream out;
   for (const Schema::Relation& rel : relations) {
      out << rel.name << std::endl;
      //out << "\tPrimary Key:";
      //for (unsigned keyId : rel.primaryKey)
      //   out << ' ' << rel.attributes[keyId].name;
      //out << std::endl;
      for (const auto& attr : rel.attributes)
         out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << std::endl;
   }
   return out.str();
}

}}

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "common/AttributeType.hpp"
#include "util/AttributeTypeConversion.hpp"

namespace dbi {
namespace sqlparser {

struct Schema {
   struct Relation {
      struct Attribute {
         std::string name;
         AttributeType type;
         uint8_t len;
         bool notNull;
		 bool primaryKey;
         Attribute() : len(~0), notNull(true), primaryKey(false) {};
		 Attribute(std::string name, AttributeType type, uint8_t len, bool notNull, bool primaryKey) : name(name), type(type), len(len), notNull(notNull), primaryKey(primaryKey) {};
         bool operator<(const Attribute& other) const {
             if (type == other.type && type == AttributeType::Char)
                return len > other.len; // Longest chars first
             else
                return typeToInt(type) < typeToInt(other.type);

         };
      };
      std::string name;
      std::vector<Schema::Relation::Attribute> attributes;
      Relation(const std::string& name) : name(name) {};
   };
   std::vector<Schema::Relation> relations;
   std::string toString() const;
};


}}



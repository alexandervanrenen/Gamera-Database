#pragma once

#include "ColumnAccessInfo.hpp"
#include "harriet/Value.hpp"
#include "harriet/VariableType.hpp"
#include <set>
#include <vector>
#include <memory>
#include <cstdint>

namespace dbi {

namespace qopt {

class ColumnAccessInfo;
class Predicate;

struct RegisterSlotInfo {
   RegisterSlotInfo(std::unique_ptr<ColumnAccessInfo> column, const std::string& identifier, const uint32_t registerIndex, const harriet::VariableType type) : column(std::move(column)), identifier(identifier), registerIndex(registerIndex), type(type) {}
   std::unique_ptr<ColumnAccessInfo> column; // If not set, the slot is not retrieved from a table
   const std::string identifier;
   const uint32_t registerIndex;
   const harriet::VariableType type;
};

class GlobalRegister {
public:
   GlobalRegister(const std::set<ColumnAccessInfo>& requiredColumns);
   void addProjectedColumn(const std::string& identifier, const harriet::VariableType& type);
   ~GlobalRegister();

   size_t size() const {return values.size();}

   harriet::Value& getSlotValue(uint32_t pos) {return values[pos];}
   const RegisterSlotInfo& getSlotInfo(uint32_t pos) const {return types[pos];}

   std::vector<uint32_t> getColumnIndexes(uint32_t tableIndex) const;

   bool hasColumn(const std::string& identifier) const;
   uint32_t getColumnIndex(const std::string& identifier) const;

   void dump(std::ostream& os) const;

private:
   std::vector<harriet::Value> values;
   std::vector<RegisterSlotInfo> types;
};

}

}

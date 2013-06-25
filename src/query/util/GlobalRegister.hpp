#pragma once

#include "ColumnAccessInfo.hpp"
#include "harriet/Value.hpp"
#include <set>
#include <vector>
#include <memory>
#include <cstdint>

namespace harriet { class Value; }

namespace dbi {

namespace qopt {

class ColumnAccessInfo;
class Predicate;

class GlobalRegister {
public:
   GlobalRegister(const std::set<ColumnAccessInfo>& requiredColumns, const std::vector<std::unique_ptr<Predicate>>& predicates);
   ~GlobalRegister();

   size_t size() const {return values.size();}

   harriet::Value& getValue(uint32_t pos) {return values[pos];}
   ColumnAccessInfo& getType(uint32_t pos) {return types[pos];}

   std::vector<uint32_t> getColumnIndexes(uint32_t tableIndex) const;
   bool hasColumn(const std::string& alias, const std::string& columnName) const;
   uint32_t getColumnIndex(const std::string& alias, const std::string& columnName) const;
   uint32_t getColumnIndex(uint32_t tableIndex, const std::string& columnName) const;

   void dump(std::ostream& os) const;

private:
   std::vector<harriet::Value> values;
   std::vector<ColumnAccessInfo> types;
};

}

}

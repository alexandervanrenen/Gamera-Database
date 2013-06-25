#include "GlobalRegister.hpp"
#include "ColumnAccessInfo.hpp"
#include "Predicate.hpp"
#include "harriet/Value.hpp"
#include "schema/RelationSchema.hpp"
#include "query/parser/Common.hpp"
#include "util/Utility.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

namespace dbi {

namespace qopt {

GlobalRegister::GlobalRegister(const set<ColumnAccessInfo>& requiredColumns)
{
   // Create register
   for(auto& iter : requiredColumns) {
      values.emplace_back(harriet::Value::createDefault(iter.columnSchema.type));
      types.emplace_back(RegisterSlotInfo{util::make_unique<ColumnAccessInfo>(iter), iter.columnReference.str(), types.size(), iter.columnSchema.type});
   }
}

void GlobalRegister::addProjectedColumn(const string& identifier, const harriet::VariableType& type)
{
   if(hasColumn(identifier))
      throw harriet::Exception("Provided name '" + identifier + "' for projection is not unique.");

   values.emplace_back(harriet::Value::createDefault(type));
   types.emplace_back(RegisterSlotInfo{nullptr, identifier, types.size(), type});
}

GlobalRegister::~GlobalRegister()
{
}

vector<uint32_t> GlobalRegister::getColumnIndexes(uint32_t tableIndex) const
{
   vector<uint32_t> result;
   for(uint32_t i=0; i<types.size(); i++)
      if(types[i].column!=nullptr && types[i].column->tableIndex==tableIndex)
         result.push_back(i);
   return result;
}

bool GlobalRegister::hasColumn(const std::string& identifier) const
{
   // Try to find any matching identifier
   for(uint32_t i=0; i<types.size(); i++)
      if(types[i].identifier==identifier)
         return true;
   return false;
}

uint32_t GlobalRegister::getColumnIndex(const std::string& identifier) const
{
   // Try to find any matching identifier
   for(uint32_t i=0; i<types.size(); i++)
      if(types[i].identifier==identifier)
         return i;
   throw;
}

void GlobalRegister::dump(std::ostream& os) const
{
   os << "-- Global Register --" << endl;
   for(uint32_t i=0; i<types.size(); i++) {
      os << i << ": ";
      if(types[i].column!=nullptr)
         types[i].column->dump(os); else
         os << types[i].type << " " << types[i].identifier;
      os << " === " << values[i] << endl;
   }
   os << "-- --------------- --" << endl;
}

}

}

#include "GlobalRegister.hpp"
#include "ColumnAccessInfo.hpp"
#include "Predicate.hpp"
#include "harriet/Value.hpp"
#include "schema/RelationSchema.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

namespace dbi {

namespace qopt {

GlobalRegister::GlobalRegister(const set<ColumnAccessInfo>& requiredColumns, const vector<unique_ptr<Predicate> >& predicates)
{
   // Merge all required columns
   set<ColumnAccessInfo> typesSet;
   for(auto& column : requiredColumns)
      typesSet.insert(column);
   for(auto& predicate : predicates)
      for(auto column : predicate->requiredColumns)
         typesSet.insert(column);

   // Create register
   for(auto& iter : typesSet) {
      values.emplace_back(harriet::Value::createDefault(iter.columnSchema.type));
      types.emplace_back(iter);
   }
}

GlobalRegister::~GlobalRegister()
{
}

vector<uint32_t> GlobalRegister::getColumnIndexes(uint32_t tableIndex) const
{
   vector<uint32_t> result;
   for(uint32_t i=0; i<types.size(); i++)
      if(types[i].tableIndex==tableIndex)
         result.push_back(i);
   return result;
}

bool GlobalRegister::hasColumn(const std::string& alias, const std::string& columnName) const
{
   // Try to find any matching identifier
   for(uint32_t i=0; i<types.size(); i++)
      if(types[i].columnSchema.name==columnName && (alias.size()==0 || alias==types[i].columnReference.tableQualifier))
         return true;
   return false;
}

uint32_t GlobalRegister::getColumnIndex(const string& alias, const string& columnName) const
{
   uint32_t resultIndex = types.size(); // invalid index

   // Try to find a matching identifier
   if(alias != "") {
      for(uint32_t i=0; i<types.size(); i++)
         if(types[i].columnSchema.name==columnName && alias==types[i].columnReference.tableQualifier) {
            if(resultIndex==types.size())
               resultIndex = i; else
               throw harriet::Exception{"ambiguous identifier '" + alias + "." + columnName + "', candidates: '" + types[i].columnReference.str() + "' or '" + types[resultIndex].columnReference.str() + "'"};
         }
   } else {
      for(uint32_t i=0; i<types.size(); i++)
         if(types[i].columnSchema.name==columnName) {
            if(resultIndex==types.size())
               resultIndex = i; else
               throw harriet::Exception{"ambiguous identifier '" + alias + "." + columnName + "', candidates: '" + types[i].columnReference.str() + "' or '" + types[resultIndex].columnReference.str() + "'"};
         }
   }

   if(resultIndex != types.size())
      return resultIndex;

   throw harriet::Exception{"unknown identifier: '" + alias + "." + columnName + "'"};
}

uint32_t GlobalRegister::getColumnIndex(uint32_t tableIndex, const std::string& columnName) const
{
   uint32_t resultIndex = types.size(); // invalid index
   for(uint32_t i=0; i<types.size(); i++) {
      if(types[i].columnSchema.name==columnName && types[i].tableIndex==tableIndex) {
         if(resultIndex==types.size())
            resultIndex = i; else
            throw harriet::Exception{"ambiguous identifier '" + to_string(tableIndex) + "." + columnName + "', candidates: '" + types[i].columnReference.str() + "' or '" + types[resultIndex].columnReference.str() + "'"};
      }
   }

   if(resultIndex != types.size())
      return resultIndex;

   throw harriet::Exception{"unknown identifier: '" + to_string(tableIndex) + "." + columnName + "'"};
}

void GlobalRegister::dump(std::ostream& os) const
{
   os << "-- Global Register --" << endl;
   for(uint32_t i=0; i<types.size(); i++) {
      os << i << ": ";
      types[i].dump(os);
      os << " === " << values[i] << endl;
   }
   os << "-- --------------- --" << endl;
}

}

}

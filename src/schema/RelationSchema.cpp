#include "RelationSchema.hpp"
#include "util/BinarySerializer.hpp"
#include <sstream>
#include <iostream>

using namespace std;

namespace dbi {

Record RelationSchema::marschall() const
{
   // Serialize relation meta data
   ostringstream out(ios::binary);
   util::writeBinary(out, sid);
   util::writeBinary(out, name);

   // Serialize its attributes
   util::writeBinary(out, attributes.size());
   for(auto& iter : attributes) {
      util::writeBinary(out, iter.name);
      util::writeBinary(out, iter.type);
      util::writeBinary(out, iter.notNull);
      util::writeBinary(out, iter.primaryKey);
   }

   // Serialize its indexes
   util::writeBinary(out, indexes.size());
   for(auto& iter : indexes) {
      util::writeBinary(out, iter.sid);
      util::writeBinary(out, iter.indexedAttribute);
      util::writeBinary(out, iter.indexType);
   }

   return Record(out.str());
}

void RelationSchema::unmarschall(const Record& record)
{
   // De-serialize relation meta data
   istringstream in(string(record.data(), record.size()), ios::binary);
   util::readBinary(sid, in);
   util::readBinary(name, in);

   // De-serialize its attributes
   size_t len;
   util::readBinary(len, in);
   attributes.resize(len);
   for(auto& iter : attributes) {
      util::readBinary(iter.name, in);
      util::readBinary(iter.type, in);
      util::readBinary(iter.notNull, in);
      util::readBinary(iter.primaryKey, in);
   }

   // De-serialize its indexes
   util::readBinary(len, in);
   indexes.resize(len);
   for(auto& iter : indexes) {
      util::readBinary(iter.sid, in);
      util::readBinary(iter.indexedAttribute, in);
      util::readBinary(iter.indexType, in);
   }

   assert(in.good());
}

const AttributeSchema& RelationSchema::getAttribute(const std::string& variableName) const
{
   for(auto& iter : attributes)
      if(iter.name == variableName)
         return iter;
   assert("unknown variable name"&&false);
   throw;
}

}

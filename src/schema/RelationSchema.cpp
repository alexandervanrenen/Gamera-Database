#include "RelationSchema.hpp"
#include "util/BinarySerializer.hpp"
#include "harriet/Expression.hpp"
#include <sstream>
#include <iostream>

using namespace std;

namespace dbi {

RelationSchema::RelationSchema()
: sid(kInvalidSegmentId)
{
}

RelationSchema::RelationSchema(const string& name, vector<AttributeSchema>&& attributes, vector<IndexSchema>&& indexes)
: sid(kInvalidSegmentId)
, name(name)
, attributes(move(attributes))
, indexes(move(indexes))
{
}

RelationSchema::RelationSchema(const Record& record)
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

RelationSchema::RelationSchema(const vector<unique_ptr<harriet::Value>>& values)
: sid(kInvalidSegmentId)
{
   for(auto& iter : values)
      attributes.push_back(AttributeSchema{"", iter->getResultType(), true, true});
}

vector<harriet::Value> RelationSchema::getTuplefromRecord(const Record& record)
{
   throw;
}

Record RelationSchema::getRecordFromTuple(const vector<harriet::Value>& tuple)
{
   throw;
}

void RelationSchema::setSegmentId(SegmentId sidIn)
{
   assert(sid==kInvalidSegmentId);
   sid = sidIn;
}

void RelationSchema::optimizePadding()
{
   throw;
}

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

}

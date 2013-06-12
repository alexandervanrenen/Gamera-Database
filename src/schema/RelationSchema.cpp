#include "RelationSchema.hpp"
#include "util/BinarySerializer.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "util/Math.hpp"
#include "util/Utility.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

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
      util::readBinary(iter.offset, in);
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

vector<unique_ptr<harriet::Value>> RelationSchema::recordToTuple(const Record& record) const
{
   vector<unique_ptr<harriet::Value>> result;
   result.reserve(attributes.size());
   for(auto& attribute : attributes)
      result.push_back(util::make_unique<harriet::Value>(attribute.type, record.data()+attribute.offset));
   return result;
}

Record RelationSchema::tupleToRecord(const vector<unique_ptr<harriet::Value>>& tuple) const
{
   assert(sid != kInvalidSegmentId);
   assert(attributes.size()==tuple.size());

   uint32_t tupleSize = 0;
   for(uint32_t i=0; i<attributes.size(); i++)
      tupleSize += attributes[i].type.length;

   vector<char> data(tupleSize);
   for(uint32_t i=0; i<tuple.size(); i++)
      tuple[i]->marschall(data.data()+attributes[i].offset);

   return Record(data);
}

void RelationSchema::setSegmentId(SegmentId sidIn)
{
   assert(sid==kInvalidSegmentId && sidIn!=kInvalidSegmentId);
   sid = sidIn;
}

void RelationSchema::optimizePadding()
{
   // First long power of two values, then the rest
   vector<uint32_t> attributeOrder(attributes.size());
   iota(attributeOrder.begin(), attributeOrder.end(), 0);
   sort(attributeOrder.begin(), attributeOrder.end(), [this](uint32_t lhs, uint32_t rhs) {
      bool lhsPowerOfTwo = util::countSetBits(attributes[lhs].type.length)==1;
      bool rhsPowerOfTwo = util::countSetBits(attributes[rhs].type.length)==1;
      if(lhsPowerOfTwo && !rhsPowerOfTwo)
         return true;
      if(!lhsPowerOfTwo && rhsPowerOfTwo)
         return false;
      return attributes[lhs].type.length>attributes[rhs].type.length;
   });

   // Set offsets
   uint32_t currentOffset = 0;
   for(uint32_t i=0; i<attributeOrder.size(); i++) {
      attributes[attributeOrder[i]].offset = currentOffset;
      currentOffset += attributes[attributeOrder[i]].type.length;
   }
}

Record RelationSchema::marschall() const
{
   assert(sid != kInvalidSegmentId);

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
      util::writeBinary(out, iter.offset);
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

void RelationSchema::dump(ostream& os) const
{
   os << "name: " << name << endl;
   os << "sid: " << sid << endl;
   for(auto& attribute : attributes)
      os << attribute.name << " " << attribute.type << " " << attribute.notNull << " " << attribute.primaryKey << " " << attribute.offset << endl;
   for(auto& index : indexes)
      os << index.sid << " " << index.indexedAttribute << " " << index.indexType << endl;
}

}

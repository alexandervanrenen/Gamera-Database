#include "SchemaManager.hpp"
#include "RelationSchema.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SchemaManager::SchemaManager(SPSegment& storage)
: storage(storage)
{
   cout << "SchemaManager::SchemaManager(SPSegment& storage)" << endl;
}

SchemaManager::~SchemaManager()
{
}

void SchemaManager::addRelation(unique_ptr<RelationSchema> relationShema)
{
   cout << "void SchemaManager::addRelation(unique_ptr<RelationSchema> relationShema)" << endl;
}

void SchemaManager::dropRelation(const string& relationName)
{
   cout << "void SchemaManager::dropRelation(const string& relationName)" << endl;
}

RelationSchema& SchemaManager::getRelationSchema(const string& relationName)
{
   cout << "RelationSchema& SchemaManager::getRelationSchema(const string& relationName)" << endl;
}

// MetadataManager::MetadataManager(SegmentManager& sm): segRelations(sm.getSPSegment(kRelationsSegmentId)), segAttributes(sm.getSPSegment(kAttributesSegmentId)) {
//     loadData();    
// }


// RelationMetadata* MetadataManager::loadRelationMetadata(const Record& r, const TupleId& tid) {
//     RelationMetadata* rm = new RelationMetadata();
//     rm->tid = tid;
//     char* data = (char*)r.data();
//     rm->segment = SegmentId(*((uint64_t*)data));
//     data += sizeof(uint64_t);
//     rm->name = string(data, r.size()-sizeof(uint64_t));
//     return rm;
// }

// Record* MetadataManager::saveRelationMetadata(RelationMetadata* rm) {
//     char* data = new char[rm->name.size()+sizeof(uint64_t)];
//     char* databegin = data;
//     *((uint64_t*)data) = rm->segment.toInteger();
//     data += sizeof(uint64_t);
//     memcpy(data, rm->name.c_str(), rm->name.size());
//     return new Record(databegin, rm->name.size()+sizeof(uint64_t));
// }

// AttributeMetadata* MetadataManager::loadAttributeMetadata(const Record& r, const TupleId& tid) {
//     AttributeMetadata* am = new AttributeMetadata();
//     am->tid = tid;
//     char* data = (char*)r.data();
//     am->indexSegment = SegmentId(*((uint64_t*)data));
//     data += sizeof(uint64_t);
//     am->relationTid = TupleId(*((uint64_t*)data));
//     data += sizeof(uint64_t);
//     am->type = intToType(*((uint8_t*)data++));
//     //cout << "reading Type: " << int(typeToInt(am->type)) << endl;
//     am->len = *((uint8_t*)data++);
//     uint8_t flags = *((uint8_t*)data++);
//     if (((flags >> 7) & 1) == 1) am->notNull = true;
//     if (((flags >> 6) & 1) == 1) am->primaryKey = true;
//     am->offset = *((uint16_t*)data);
//     data += sizeof(uint16_t);
//     am->name = string(data, r.size()-2*sizeof(uint64_t)-3*sizeof(uint8_t)-sizeof(uint16_t));
//     return am;
// }

// Record* MetadataManager::saveAttributeMetadata(AttributeMetadata* am) {
//     uint64_t size = am->name.size()+2*sizeof(uint64_t)+3*sizeof(uint8_t)+sizeof(uint16_t);
//     char* data = new char[size];
//     char* databegin = data;
//     *((uint64_t*)data) = am->indexSegment.toInteger();
//     data += sizeof(uint64_t);
//     *((uint64_t*)data) = am->relationTid.toInteger();
//     data += sizeof(uint64_t);
//     *((uint8_t*)data++) = typeToInt(am->type);
//     //cout << "Type (should be): " << int(typeToInt(am->type)) << endl;
//     //cout << "Type: " << int(*((uint8_t*)(data-1))) << endl;
//     *((uint8_t*)data++) = am->len;
//     uint8_t flags = 0;
//     if (am->notNull) flags |= (1<<7);
//     if (am->primaryKey) flags |= (1<<6);
//     *((uint8_t*)data++) = flags;
//     *((uint16_t*)data) = am->offset;
//     data += sizeof(uint16_t);
//     memcpy(data, am->name.c_str(), am->name.size());
//     return new Record(databegin, size);
// }

// void MetadataManager::loadData() {
//     TableScanOperator relScan{segRelations};
//     relScan.open();
//     while (relScan.next()) {
//         const pair<TupleId, Record>& p = relScan.getOutput();
//         RelationMetadata* rm = loadRelationMetadata(p.second, p.first);
//         relations.insert({rm->name, rm});
//         relationsByTid.insert({p.first, rm});
//     }
//     relScan.close();
//     TableScanOperator attrScan{segAttributes};
//     attrScan.open();
//     while (attrScan.next()) {
//         const pair<TupleId, Record>& p = attrScan.getOutput();
//         AttributeMetadata* am = loadAttributeMetadata(p.second, p.first);
//         RelationMetadata* rm = relationsByTid[am->relationTid];
//         assert(rm != nullptr);
//         rm->attributes.push_back(am);
//         attributes.insert({{rm->name, am->name}, am});
//     }
//     attrScan.close();

//     for (auto p : relations) {
//         calculateRelationIndexes(p.second);
//     }
// }

// void MetadataManager::calculateRelationIndexes(RelationMetadata* rm) {
//     relationIndexes[rm->name].clear();
//     unordered_map<SegmentId, vector<AttributeMetadata*>> map;
//     for (AttributeMetadata* am : rm->attributes) {
//         if (am->indexSegment != kInvalidSegmentId)
//             map[am->indexSegment].push_back(am);
//     }
//     for (auto p : map) {
//         IndexMetadata* idx = new IndexMetadata(rm, p.first, p.second);
//         relationIndexes[rm->name].push_back(idx);
//     }
// }

// void MetadataManager::addRelation(RelationSchema& rel) {
//     reorderRelation(rel);
//     RelationMetadata* rm = new RelationMetadata();
//     rm->name = rel.name;
//     Record* relRec = saveRelationMetadata(rm);
//     TupleId relTid = segRelations.insert(*relRec); // Save record in SPSegment
//     rm->tid = relTid;
//     relationsByTid.insert({relTid, rm});
//     relations.insert({rm->name, rm});
//     uint16_t offset = 0;
//     for (auto a: rel.attributes) {
//         AttributeMetadata* am = new AttributeMetadata(a.name, a.type, a.len, a.notNull, a.primaryKey);
//         am->relationTid = relTid;
//         am->offset = offset;
//         rm->attributes.push_back(am);
//         TupleId attrTid = segAttributes.insert(*(saveAttributeMetadata(am))); // Save record in SPSegment
//         am->tid = attrTid;
//         attributes.insert({{rm->name, am->name}, am});
//         if (a.type == AttributeType::Integer)
//             offset += sizeof(uint64_t);
//         else
//             offset += a.len;
//     }
//     calculateRelationIndexes(rm);
// }

// void MetadataManager::saveRelation(RelationMetadata* rm) {
//     Record* r = saveRelationMetadata(rm);
//     if (rm->tid != kInvalidTupleId)
//         segRelations.update(rm->tid, *r);
//     else {
//         TupleId tid = segRelations.insert(*r);
//         rm->tid = tid;
//     }
// }

// void MetadataManager::saveAttribute(AttributeMetadata* am) {
//     Record* r = saveAttributeMetadata(am);
//     if (am->tid != kInvalidTupleId)
//         segAttributes.update(am->tid, *r);
//     else {
//         TupleId tid = segAttributes.insert(*r);
//         am->tid = tid;
//     }
// }


// void MetadataManager::setSegment(const string& relationName, SegmentId sid) {
//     auto it = relations.find(relationName);
//     assert(it != relations.end());
//     RelationMetadata* rm = it->second;
//     rm->segment = sid;
//     saveRelation(rm);
// }

// SegmentId MetadataManager::getSegmentForRelation(const string& name) {
//     auto it = relations.find(name);
//     assert(it != relations.end());
//     RelationMetadata* rm = it->second;
//     return rm->segment;
// }


// AttributeType MetadataManager::getTypeForAttribute(const string& relationName, const string& attributeName) {
//     auto it = attributes.find({relationName, attributeName});
//     assert(it != attributes.end());
//     return it->second->type;
// }

// void MetadataManager::addIndex(const string& relationName, SegmentId sid, const vector<string>& attr) {
//     auto it = relations.find(relationName);
//     assert(it != relations.end());
//     RelationMetadata* rm = it->second;
//     for (string a: attr) {
//         auto it = attributes.find({relationName, a});
//         assert(it != attributes.end());
//         it->second->indexSegment = sid;
//         saveAttribute(it->second);
//     }
//     calculateRelationIndexes(rm);
// }

// MetadataManager::RelationIndexes MetadataManager::getRelationIndexes(const string& relationName) {
//     auto it = relationIndexes.find(relationName);
//     assert(it != relationIndexes.end());
//     return it->second;
// }

// void MetadataManager::reorderRelation(RelationSchema& r) {
//     stable_sort(r.attributes.begin(), r.attributes.end());
// }

// uint16_t MetadataManager::getAttributeOffset(const string& relationName, const string& attributeName) {
//     auto it = attributes.find({relationName, attributeName});
//     assert(it != attributes.end());
//     return it->second->offset;
// }

}

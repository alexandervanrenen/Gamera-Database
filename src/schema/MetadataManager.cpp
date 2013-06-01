#include "MetadataManager.hpp"
#include "operator/TableScanOperator.hpp"
#include <iostream>

using namespace std;

namespace dbi {

MetadataManager::MetadataManager(SegmentManager& sm): segRelations(sm.getSPSegment(kRelationsSegmentId)), segAttributes(sm.getSPSegment(kAttributesSegmentId)) {
    loadData();    
}


RelationMetadata* MetadataManager::loadRelationMetadata(const Record& r, const TupleId& tid) {
    RelationMetadata* rm = new RelationMetadata();
    rm->tid = tid;
    char* data = (char*)r.data();
    rm->segment = SegmentId(*((uint64_t*)data));
    data += sizeof(uint64_t);
    rm->name = std::string(data, r.size()-sizeof(uint64_t));
    return rm;
}

Record* MetadataManager::saveRelationMetadata(RelationMetadata* rm) {
    char* data = new char[rm->name.size()+sizeof(uint64_t)];
    char* databegin = data;
    *((uint64_t*)data) = rm->segment.toInteger();
    data += sizeof(uint64_t);
    memcpy(data, rm->name.c_str(), rm->name.size());
    return new Record(databegin, rm->name.size()+sizeof(uint64_t));
}

AttributeMetadata* MetadataManager::loadAttributeMetadata(const Record& r, const TupleId& tid) {
    AttributeMetadata* am = new AttributeMetadata();
    am->tid = tid;
    char* data = (char*)r.data();
    am->indexSegment = SegmentId(*((uint64_t*)data));
    data += sizeof(uint64_t);
    am->relationTid = TupleId(*((uint64_t*)data));
    data += sizeof(uint64_t);
    am->len = *((uint8_t*)data++);
    uint8_t flags = *((uint8_t*)data++);
    if (((flags >> 7) & 1) == 1) am->notNull = true;
    if (((flags >> 6) & 1) == 1) am->primaryKey = true;
    am->name = std::string(data, r.size()-2*sizeof(uint64_t)-2*sizeof(uint8_t));
    return am;
}

Record* MetadataManager::saveAttributeMetadata(AttributeMetadata* am) {
    char* data = new char[am->name.size()+2*sizeof(uint64_t)+2*sizeof(uint8_t)];
    char* databegin = data;
    *((uint64_t*)data) = am->indexSegment.toInteger();
    data += sizeof(uint64_t);
    *((uint64_t*)data) = am->relationTid.toInteger();
    data += sizeof(uint64_t);
    uint8_t flags = 0;
    if (am->notNull) flags |= (1<<7);
    if (am->primaryKey) flags |= (1<<6);
    *((uint8_t*)data++) = flags;
    memcpy(data, am->name.c_str(), am->name.size());
    return new Record(databegin, am->name.size()+2*sizeof(uint64_t)+2*sizeof(uint8_t));
}

void MetadataManager::loadData() {
    TableScanOperator relScan{segRelations};
    relScan.open();
    while (relScan.next()) {
        const pair<TupleId, Record>& p = relScan.getOutput();
        RelationMetadata* rm = loadRelationMetadata(p.second, p.first);
        relations.insert({rm->name, rm});
        relationsByTid.insert({p.first, rm});
    }
    relScan.close();
    TableScanOperator attrScan{segAttributes};
    attrScan.open();
    while (attrScan.next()) {
        const pair<TupleId, Record>& p = attrScan.getOutput();
        AttributeMetadata* am = loadAttributeMetadata(p.second, p.first);
        RelationMetadata* rm = relationsByTid[am->relationTid];
        assert(rm != nullptr);
        rm->attributes.push_back(am);
        attributes.insert({{rm->name, am->name}, am});
    }
    attrScan.close();
}

void MetadataManager::addRelation(sqlRelation rel) {
    RelationMetadata* rm = new RelationMetadata();
    rm->name = rel.name;
    Record* relRec = saveRelationMetadata(rm);
    TupleId relTid = segRelations.insert(*relRec);
    rm->tid = relTid;
    relationsByTid.insert({relTid, rm});
    relations.insert({rm->name, rm});
    for (sqlAttribute a: rel.attributes) {
        AttributeMetadata* am = new AttributeMetadata(a.name, a.type, a.len, a.notNull, a.primaryKey);
        am->relationTid = relTid;
        rm->attributes.push_back(am);
        TupleId attrTid = segAttributes.insert(*(saveAttributeMetadata(am)));
        am->tid = attrTid;
        attributes.insert({{rm->name, am->name}, am});
    }
}

void MetadataManager::saveRelation(RelationMetadata* rm) {
    Record* r = saveRelationMetadata(rm);
    if (rm->tid != kInvalidTupleId)
        segRelations.update(rm->tid, *r);
    else {
        TupleId tid = segRelations.insert(*r);
        rm->tid = tid;
    }
}

void MetadataManager::saveAttribute(AttributeMetadata* am) {
    Record* r = saveAttributeMetadata(am);
    if (am->tid != kInvalidTupleId)
        segAttributes.update(am->tid, *r);
    else {
        TupleId tid = segAttributes.insert(*r);
        am->tid = tid;
    }
}


void MetadataManager::setSegment(std::string relationName, SegmentId sid) {
    auto it = relations.find(relationName);
    assert(it != relations.end());
    RelationMetadata* rm = it->second;
    rm->segment = sid;
    saveRelation(rm);
}

SegmentId MetadataManager::getSegmentForRelation(const std::string name) {
    auto it = relations.find(name);
    assert(it != relations.end());
    RelationMetadata* rm = it->second;
    return rm->segment;
}

}

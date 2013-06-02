#include "MetadataManager.hpp"
#include "operator/TableScanOperator.hpp"
#include <iostream>

using namespace std;

namespace dbi {

MetadataManager::MetadataManager(SegmentManager& sm): segRelations(sm.getSPSegment(kRelationsSegmentId)), segAttributes(sm.getSPSegment(kAttributesSegmentId)) {
    loadData();    
}

// Construct RelationMetadata object from Record
RelationMetadata* MetadataManager::loadRelationMetadata(const Record& r, const TupleId& tid) {
    RelationMetadata* rm = new RelationMetadata();
    rm->tid = tid;
    char* data = (char*)r.data();
    rm->segment = SegmentId(*((uint64_t*)data));
    data += sizeof(uint64_t);
    rm->name = std::string(data, r.size()-sizeof(uint64_t));
    return rm;
}

// Construct Record from RelationMetadata object
Record* MetadataManager::saveRelationMetadata(RelationMetadata* rm) {
    char* data = new char[rm->name.size()+sizeof(uint64_t)];
    char* databegin = data;
    *((uint64_t*)data) = rm->segment.toInteger();
    data += sizeof(uint64_t);
    memcpy(data, rm->name.c_str(), rm->name.size());
    return new Record(databegin, rm->name.size()+sizeof(uint64_t));
}

// Used for easier retrieval of information from record
struct AttrHeader {
    uint64_t relationTid;
    uint16_t type;
    uint8_t flags;
    uint16_t len;
    uint16_t offset;
    uint8_t numberOfIndexSegments;
};

static_assert(sizeof(AttrHeader) == 24, "Padding is wrong"); // Assume padding of 4 bytes

// Get data from record and construct AttributeMetadata object
AttributeMetadata* MetadataManager::loadAttributeMetadata(const Record& r, const TupleId& tid) {
    AttributeMetadata* am = new AttributeMetadata();
    am->tid = tid;
    char* data = (char*)r.data();
    AttrHeader* header = (AttrHeader*)data;
    am->relationTid = TupleId(header->relationTid);
    am->type = intToType(header->type);
    am->len = header->len;
    uint8_t flags = header->flags;
    if (((flags) & 1) == 1) am->notNull = true;
    if (((flags >> 1) & 1) == 1) am->primaryKey = true;
    am->offset = header->offset;
    data += sizeof(AttrHeader)-4;
    for (uint8_t i=0; i < header->numberOfIndexSegments; i++) {
        am->indexSegments.push_back(SegmentId(*((uint64_t*)data)));
        data += sizeof(uint64_t);
    }
    am->name = std::string(data, r.size()-sizeof(AttrHeader)+4-sizeof(uint64_t)*header->numberOfIndexSegments);
    return am;
}

// Construct record from AttributeMetdata object
Record* MetadataManager::saveAttributeMetadata(AttributeMetadata* am) {
    uint64_t size = am->name.size()+sizeof(AttrHeader)-4+am->indexSegments.size()*sizeof(uint64_t);
    char* data = new char[size];
    char* databegin = data;
    AttrHeader* header = (AttrHeader*)data;
    header->relationTid = am->relationTid.toInteger();
    header->type = typeToInt(am->type);
    header->len = am->len;
    uint8_t flags = 0;
    if (am->notNull) flags |= (1);
    if (am->primaryKey) flags |= (1<<1);
    header->flags = flags;
    header->offset = am->offset;
    header->numberOfIndexSegments = am->indexSegments.size();
    data += sizeof(AttrHeader)-4;
    for (SegmentId id : am->indexSegments) {
        *((uint64_t*)data) = id.toInteger();
        data += sizeof(uint64_t);
    }
    memcpy(data, am->name.c_str(), am->name.size());
    return new Record(databegin, size);
}

// Load data from SPSegments
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

    for (auto p : relations) {
        calculateRelationIndexes(p.second);
    }
}

void MetadataManager::calculateRelationIndexes(RelationMetadata* rm) {
    relationIndexes[rm->name].clear();
    std::unordered_map<SegmentId, std::vector<AttributeMetadata*>> map;
    for (AttributeMetadata* am : rm->attributes) {
        for (SegmentId id: am->indexSegments) {
            map[id].push_back(am);
        }
    }
    for (auto p : map) {
        IndexMetadata* idx = new IndexMetadata(rm, p.first, p.second);
        relationIndexes[rm->name].push_back(idx);
    }
}

void MetadataManager::addRelation(sqlRelation rel) {
    reorderRelation(rel);
    RelationMetadata* rm = new RelationMetadata();
    rm->name = rel.name;
    Record* relRec = saveRelationMetadata(rm);
    TupleId relTid = segRelations.insert(*relRec); // Save record in SPSegment
    rm->tid = relTid;
    relationsByTid.insert({relTid, rm});
    relations.insert({rm->name, rm});
    uint16_t offset = 0;
    for (sqlAttribute a: rel.attributes) {
        AttributeMetadata* am = new AttributeMetadata(a.name, a.type, a.len, a.notNull, a.primaryKey);
        am->relationTid = relTid;
        am->offset = offset;
        rm->attributes.push_back(am);
        TupleId attrTid = segAttributes.insert(*(saveAttributeMetadata(am))); // Save record in SPSegment
        am->tid = attrTid;
        attributes.insert({{rm->name, am->name}, am});
        if (a.type == AttributeType::Integer)
            offset += sizeof(uint64_t);
        else
            offset += a.len;
    }
    calculateRelationIndexes(rm);
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


void MetadataManager::setSegment(const std::string relationName, SegmentId sid) {
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


AttributeType MetadataManager::getTypeForAttribute(const std::string relationName, const std::string attributeName) {
    auto it = attributes.find({relationName, attributeName});
    assert(it != attributes.end());
    return it->second->type;
}

void MetadataManager::addIndex(const std::string relationName, SegmentId sid, std::vector<std::string> attr) {
    auto it = relations.find(relationName);
    assert(it != relations.end());
    RelationMetadata* rm = it->second;
    for (std::string a: attr) {
        auto it = attributes.find({relationName, a});
        assert(it != attributes.end());
        it->second->indexSegments.push_back(sid);
        saveAttribute(it->second);
    }
    calculateRelationIndexes(rm);
}

MetadataManager::RelationIndexes MetadataManager::getRelationIndexes(const std::string relationName) {
    auto it = relationIndexes.find(relationName);
    assert(it != relationIndexes.end());
    return it->second;
}

void MetadataManager::reorderRelation(sqlRelation& r) {
    std::stable_sort(r.attributes.begin(), r.attributes.end());
}

uint16_t MetadataManager::getAttributeOffset(const std::string relationName, const std::string attributeName) {
    auto it = attributes.find({relationName, attributeName});
    assert(it != attributes.end());
    return it->second->offset;
}

}

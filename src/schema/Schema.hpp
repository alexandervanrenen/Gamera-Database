#pragma once

#include "common/AttributeType.hpp"
#include "common/SegmentId.hpp"
#include "common/TupleId.hpp"
#include "common/Config.hpp"

namespace dbi {

struct AttributeMetadata {
    std::string name;
    AttributeType type;
    uint8_t len;
    uint16_t offset;
    bool notNull;
    bool primaryKey;
    TupleId tid; // not saved
    TupleId relationTid;
    SegmentId indexSegment;
    AttributeMetadata() : len(~0), notNull(true), primaryKey(false), tid(kInvalidTupleId), relationTid(kInvalidTupleId), indexSegment() {};
    AttributeMetadata(std::string name, AttributeType type, uint8_t len, bool notNull, bool primaryKey) : name(name), type(type), len(len), notNull(notNull), primaryKey(primaryKey), indexSegment(-1) {};
};

struct RelationMetadata {
    std::string name;
    SegmentId segment;
    TupleId tid; // not saved
    std::vector<AttributeMetadata*> attributes;
    RelationMetadata() : segment(-1) {};
    RelationMetadata(const std::string& name) : name(name), segment(-1), tid(kInvalidTupleId) {}
};

}

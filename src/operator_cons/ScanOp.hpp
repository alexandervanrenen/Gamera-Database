#pragma once

#include "COperator.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "schema/RelationSchema.hpp"
#include "segment_manager/SPSegment.hpp"
#include <vector>

namespace dbi {
    
    class ScanOp :public COperator{
    public:
        ScanOp(const SPSegment& relation, uint16_t writeStartIndex, const RelationSchema& relationSchema, std::vector<harriet::Value>& registerz)
                : COperator(registerz), relation(relation), relationSchema(relationSchema) {
            this->writeStartIndex = writeStartIndex;
        }
                
        ~ScanOp(){
            
        }
        
        virtual void open() = 0;

        virtual void close() = 0;

        virtual bool next() = 0;
        
    protected:
        const SPSegment& relation;
        uint16_t writeStartIndex;
        const RelationSchema& relationSchema;
    };
}


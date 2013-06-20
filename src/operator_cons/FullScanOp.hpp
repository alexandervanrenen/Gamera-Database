#pragma once

#include "common/TupleId.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "operator_cons/ScanOp.hpp"
#include "schema/RelationSchema.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include "segment_manager/Record.hpp"
#include "segment_manager/SPSegment.hpp"
#include <vector>

namespace dbi {

    class FullScanOp :public ScanOp {
    public:

        FullScanOp(const SPSegment& relation, uint16_t writeStartIndex, const RelationSchema& relationSchema, std::vector<harriet::Value>& registerz);

        virtual void open();

        virtual void close();

        virtual bool next();
    private:

        PageIdIterator pageIdIter;        
        std::vector<std::pair<TupleId, Record>> pageEntries; 
        std::vector<std::pair<TupleId, Record>>::iterator pageEntryIter;
        
        virtual void initEntries();
        

    };

}


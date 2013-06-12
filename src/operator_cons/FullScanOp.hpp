#pragma once

#include "operator_cons/ScanOp.hpp"
#include "harriet/ScriptLanguage.hpp"
#include <vector>
#include "segment_manager/SPSegment.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include "common/TupleId.hpp"
#include "segment_manager/Record.hpp"
#include "schema/RelationSchema.hpp"

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


#include "FullScanOp.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "harriet/Value.hpp"

using namespace std;

namespace dbi {

    FullScanOp::FullScanOp(const SPSegment& relation, uint16_t writeStartIndex, const RelationSchema& relationSchema, std::vector<harriet::Value>& registerz)
        : ScanOp(relation, writeStartIndex, relationSchema, registerz), pageIdIter(relation.endPageId()) {
    }

    void FullScanOp::open() {
        pageIdIter = relation.beginPageId();
        initEntries();
    }

    void FullScanOp::close() {

    }

    bool FullScanOp::next() {
        if (pageEntryIter == pageEntries.end()) {
            pageIdIter++;
            if (pageIdIter == relation.endPageId())
                return false;
            else
                initEntries();
        }
        auto recordValues = relationSchema.recordToTuple((pageEntryIter++)->second);
        for (uint16_t i = 0; i < recordValues.size(); i++) {
            registerz[writeStartIndex + i] = move(recordValues[i]); // May be broken ?
        }
        return true;
    }

    void FullScanOp::initEntries() {
        pageEntries = relation.getAllRecordsOfPage(*pageIdIter);
        pageEntryIter = pageEntries.begin();
    }
}
